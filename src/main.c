#include "main.h"

// This project headers.
#include "algebra.h"
#include "input.h"
#include "mesh.h"
#include "render.h"

// Third-party headers.
#include <SDL.h>

// Standard headers.

#define RGBA_COLOR_BLACK            0x00, 0x00, 0x00, 0xff
#define RGBA_COLOR_WHITE            0xff, 0xff, 0xff, 0xff
#define RGBA_COLOR_TIFFANY_BLUE     0x75, 0xdb, 0xcd, 0xff
#define RGBA_COLOR_ATOMIC_TANGERINE 0xfa, 0xa3, 0x81, 0xff
#define RGBA_COLOR_MAGENTA          0xff, 0x00, 0xff, 0xff

// https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
// TODO: Move it to some other file.
SDL_bool IntersectRaySegmentSphere(const Vec3* o, const Vec3* d, const Vec3* so, float radius2, Vec3* ip)
{
    const Vec3 d_normalized = Vec3Normalize(d);

    const Vec3 m = Vec3Sub(o, so);
    const float b = Vec3Dot(&m, &d_normalized);
    const float c = Vec3Dot(&m, &m) - radius2;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return SDL_FALSE;
    const float discr = b * b - c;

    if (discr < 0.0f)
        return SDL_FALSE;

    float t = -b - SDL_sqrtf(discr);
    t = SDL_max(0.0f, t);

    const Vec3 _d = Vec3ScalarMul(&d_normalized, t);
    *ip = Vec3Add(o, &_d);

    if (t > Vec3Length(d))
        return SDL_FALSE;

    return SDL_TRUE;
}

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    Mesh** mesh_pointers[2];
    for (size_t i = 0; i < sizeof(mesh_pointers) / sizeof(Mesh*); ++i)
        mesh_pointers[i] = NULL;

    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL initialization failure: %s\n", SDL_GetError());
            goto Exit;
        }

        window = SDL_CreateWindow("Rastrum", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL window creation failure: %s\n", SDL_GetError());
            goto Exit;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL renderer creation failure: %s\n", SDL_GetError());
            goto Exit;
        }
    }

    // Setup meshes.
    //

    Mesh* rocket =    MeshLoadFromObj("assets/plane.obj");
    Mesh* ball =      MeshLoadFromObj("assets/ball.obj");
    //Mesh* wall_side = MeshLoadFromObj("assets/wall_side.obj");

    mesh_pointers[0] = &rocket;
    mesh_pointers[1] = &ball;
    //mesh_pointers[2] = &wall_side;

    for (size_t i = 0; i < sizeof(mesh_pointers) / sizeof(Mesh*); ++i) {
        if (mesh_pointers[i] == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mesh loading failure.\n");
            goto Exit;
        }
    }

    rocket->transform->m[1][1] = 0;
    rocket->transform->m[1][2] = -1.0f;
    rocket->transform->m[2][1] = 1.0f;
    rocket->transform->m[2][2] = 0;
    rocket->transform->m[3][2] = 1.0f;

    //// Rotation.
    //wall_side->transform->m[0][0] =  0.0f;
    //wall_side->transform->m[0][2] = -1.0f;
    //wall_side->transform->m[2][0] =  1.0f;
    //wall_side->transform->m[2][2] =  0.0f;

    //// Translation.
    //wall_side->transform->m[3][2] = -20.0f;
    //wall_side->transform->m[3][0] =  10.5f;

    // Translation.
    ball->transform->m[3][2] = -4.0f;

    // Setup camera.
    //

    const float camera_angle = -60.0f;
    const Mat4x4 camera = {
        .m = {
            {1,  0,  0,  0},
            { 0,  SDL_cosf(DEGREES_TO_RADIANS(camera_angle)), SDL_sinf(DEGREES_TO_RADIANS(camera_angle)), 0},
            { 0, -SDL_sinf(DEGREES_TO_RADIANS(camera_angle)), SDL_cosf(DEGREES_TO_RADIANS(camera_angle)), 0},
            {0,  7, 4, 1},
        }
    };

    RenderSetCamera(&camera);
    RenderSetRenderer(renderer);

    SDL_bool quit = SDL_FALSE;
    while (quit == SDL_FALSE) {
        // Input.
        //

        quit = InputProcess(rocket);


        // Collision.
        //

        SDL_bool collides = SDL_FALSE;

        for (size_t i = 0; i < sizeof(mesh_pointers) / sizeof(Mesh*); ++i) {
            const Mesh* mesh = *mesh_pointers[i];
            if (mesh == ball)
                continue;

            const Vec3 ball_origin = {
                .x = ball->transform->m[3][0],
                .y = ball->transform->m[3][1],
                .z = ball->transform->m[3][2],
            };
            const float ball_radius = 1.0f;

            for (uint_fast32_t j = 0; j < mesh->number_of_faces; ++j) {
                SDL_bool outside_plane    = SDL_FALSE;
                SDL_bool outside_vertices = SDL_FALSE;
                SDL_bool outside_edges    = SDL_FALSE;

                const Vec3 a = Vec3MulByMat4x4(&mesh->vertices[mesh->indices[j * 3 + 0]], mesh->transform);
                const Vec3 b = Vec3MulByMat4x4(&mesh->vertices[mesh->indices[j * 3 + 1]], mesh->transform);
                const Vec3 c = Vec3MulByMat4x4(&mesh->vertices[mesh->indices[j * 3 + 2]], mesh->transform);

                // Check for plane intersection.
                //

                const Vec3 u = Vec3Sub(&b, &a);
                const Vec3 v = Vec3Sub(&c, &a);
                Vec3 abc_normal = Vec3Cross(&u, &v);
                abc_normal = Vec3Normalize(&abc_normal);

                if (SDL_fabsf(abc_normal.y) > 0.1f)
                    continue;

                Vec3 abc_average = Vec3Add(&a, &b);
                abc_average = Vec3Add(&abc_average, &c);
                abc_average = Vec3ScalarMul(&abc_average, -(1.0f / 3.0f));
                const float d = Vec3Dot(&abc_average, &abc_normal);

                const float point_to_plane_distance = Vec3Dot(&abc_normal, &ball_origin) + d;

                if (SDL_fabsf(point_to_plane_distance) > ball_radius)
                    continue;

                // Check for vertices intersection.
                //

                const Vec3 a_to_ball_origin = Vec3Sub(&a, &ball_origin);
                const SDL_bool outside_a    = Vec3Dot(&a_to_ball_origin, &a_to_ball_origin) > ball_radius;
                const Vec3 b_to_ball_origin = Vec3Sub(&b, &ball_origin);
                const SDL_bool outside_b    = Vec3Dot(&b_to_ball_origin, &b_to_ball_origin) > ball_radius;
                const Vec3 c_to_ball_origin = Vec3Sub(&c, &ball_origin);
                const SDL_bool outside_c    = Vec3Dot(&c_to_ball_origin, &c_to_ball_origin) > ball_radius;

                if (outside_a && outside_b && outside_c)
                    outside_vertices = SDL_TRUE;

                // Check for edges intersections.
                //

                const Vec3 bma = Vec3Sub(&b, &a);
                const Vec3 cmb = Vec3Sub(&c, &b);
                const Vec3 amc = Vec3Sub(&a, &c);

                Vec3 ip;

                if (!IntersectRaySegmentSphere(&a, &bma, &ball_origin, ball_radius, &ip) &&
                    !IntersectRaySegmentSphere(&b, &cmb, &ball_origin, ball_radius, &ip) &&
                    !IntersectRaySegmentSphere(&c, &amc, &ball_origin, ball_radius, &ip))
                    outside_edges = SDL_TRUE;

                if (outside_edges && outside_vertices)
                    continue;

                collides = SDL_TRUE;
            }
        }


        // Render.
        //

        RenderBackgroud(RGBA_COLOR_TIFFANY_BLUE);
        for (size_t i = 0; i < sizeof(mesh_pointers) / sizeof(Mesh*); ++i)
            if (collides == SDL_TRUE && *mesh_pointers[i] == ball)
                RenderMeshWireframe(*mesh_pointers[i], RGBA_COLOR_MAGENTA);
            else
                RenderMeshWireframe(*mesh_pointers[i], RGBA_COLOR_ATOMIC_TANGERINE);

        SDL_RenderPresent(renderer);
    }

Exit:
    // Clean up.
    //

    for (size_t i = 0; i < sizeof(mesh_pointers) / sizeof(Mesh*); ++i)
        if (mesh_pointers[i] != NULL)
            MeshFree(*mesh_pointers[i]);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL)   SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
