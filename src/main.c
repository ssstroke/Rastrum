#include "main.h"

// This project headers.
#include "algebra.h"
#include "game.h"
#include "input.h"
#include "mesh.h"
#include "render.h"

// Third-party headers.
#include <SDL.h>

// Standard headers.

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    Mesh* mesh_pointers[3];
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

    Mesh* rocket = MeshLoadFromObj("assets/rocket.obj");
    Mesh* ball = MeshLoadFromObj("assets/sphere_cool.obj");
    Mesh* entity = MeshLoadFromObj("assets/rocket.obj");

    mesh_pointers[0] = rocket;
    mesh_pointers[1] = ball;
    mesh_pointers[2] = entity;

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

    ball->transform->m[3][2] = -1.0f;

    entity->transform->m[1][1] = 0;
    entity->transform->m[1][2] = -1.0f;
    entity->transform->m[2][1] = 1.0f;
    entity->transform->m[2][2] = 0;
    entity->transform->m[3][2] = -5.0f;

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

    Vec2 ball_direction = {
        .x = 0.0f,
        .z = 1.0f,
    };

    SDL_bool quit = SDL_FALSE;
    while (quit == SDL_FALSE) {
        // Input.
        //
        quit = InputProcess(rocket);


        // Update game.
        //
        GameUpdate(mesh_pointers, sizeof(mesh_pointers) / sizeof(Mesh*), ball, &ball_direction);

        // Render.
        //
        RenderBackgroud(RGBA_COLOR_GUNMETAL);
        for (size_t i = 0; i < sizeof(mesh_pointers) / sizeof(Mesh*); ++i)
            RenderMeshWireframe(mesh_pointers[i], mesh_pointers[i]->color);

        SDL_RenderPresent(renderer);
    }

Exit:
    // Clean up.
    //

    for (size_t i = 0; i < sizeof(mesh_pointers) / sizeof(Mesh*); ++i)
        if (mesh_pointers[i] != NULL)
            MeshFree(mesh_pointers[i]);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL)   SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
