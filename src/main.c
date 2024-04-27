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

#define TARGET_FPS 144.0f

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    Mesh* meshes[4];
    for (size_t i = 0; i < sizeof(meshes) / sizeof(Mesh*); ++i)
        meshes[i] = NULL;

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

    RenderSetRenderer(renderer);

    // Load meshes.
    //

    Mesh* mesh_wall_side = MeshLoadFromObj("assets/wall_side.obj");
    meshes[0] = mesh_wall_side;

    Mesh* mesh_wall_back = MeshLoadFromObj("assets/wall_back.obj");
    meshes[1] = mesh_wall_back;

    Mesh* mesh_ball = MeshLoadFromObj("assets/ball.obj");
    meshes[2] = mesh_ball;

    Mesh* mesh_rectangle = MeshLoadFromObj("assets/rectangle.obj");
    meshes[3] = mesh_rectangle;

    for (size_t i = 0; i < sizeof(meshes) / sizeof(Mesh*); ++i) {
        if (meshes[i] == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mesh loading failure.\n");
            goto Exit;
        }
    }

    // Setup scene objects.
    //

    GameObject objects[3 + 1 + 1 + 8];

    {
        // Left wall.
        objects[0].mesh = mesh_wall_side;
        objects[0].active = SDL_TRUE;
        objects[0].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[0], -9.0f, 0, -14.0f);

        // Right wall.
        objects[1].mesh = mesh_wall_side;
        objects[1].active = SDL_TRUE;
        objects[1].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[1], 9.0f, 0, -14.0f);

        // Back wall.
        objects[2].mesh = mesh_wall_back;
        objects[2].active = SDL_TRUE;
        objects[2].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[2], 0, 0, -27.0f);

        // Ball.
        objects[3].mesh = mesh_ball;
        objects[3].active = SDL_TRUE;
        objects[3].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[3], 0, 0, -4.0f);

        // Player.
        objects[4].mesh = mesh_rectangle;
        objects[4].active = SDL_TRUE;
        objects[4].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[4], 0, 0, -1.0f);

        // Entities.
        objects[5].mesh = mesh_rectangle;
        objects[5].active = SDL_TRUE;
        objects[5].entity = SDL_TRUE;
        objects[5].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[5], -6.0f, 0, -24.0f);
        objects[6].mesh = mesh_rectangle;
        objects[6].active = SDL_TRUE;
        objects[6].entity = SDL_TRUE;
        objects[6].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[6], -2.0f, 0, -24.0f);
        objects[7].mesh = mesh_rectangle;
        objects[7].active = SDL_TRUE;
        objects[7].entity = SDL_TRUE;
        objects[7].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[7], 2.0f, 0, -24.0f);
        objects[8].mesh = mesh_rectangle;
        objects[8].active = SDL_TRUE;
        objects[8].entity = SDL_TRUE;
        objects[8].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[8], 6.0f, 0, -24.0f);
        objects[9].mesh = mesh_rectangle;
        objects[9].active = SDL_TRUE;
        objects[9].entity = SDL_TRUE;
        objects[9].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[9], -6.0f, 0, -22.0f);
        objects[10].mesh = mesh_rectangle;
        objects[10].active = SDL_TRUE;
        objects[10].entity = SDL_TRUE;
        objects[10].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[10], -2.0f, 0, -22.0f);
        objects[11].mesh = mesh_rectangle;
        objects[11].active = SDL_TRUE;
        objects[11].entity = SDL_TRUE;
        objects[11].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[11], 2.0f, 0, -22.0f);
        objects[12].mesh = mesh_rectangle;
        objects[12].active = SDL_TRUE;
        objects[12].entity = SDL_TRUE;
        objects[12].transform = malloc(sizeof(Mat4x4));
        GameObjectSetTransformPosition(&objects[12], 6.0f, 0, -22.0f);
    }
    
    GameObject* player = &objects[4];
    GameObject* ball = &objects[3];

    // Setup camera.
    //

    const float camera_angle = -45.0f;
    const Mat4x4 camera = {
        .m = {
            {1,  0,  0,  0},
            { 0,  SDL_cosf(DEGREES_TO_RADIANS(camera_angle)), SDL_sinf(DEGREES_TO_RADIANS(camera_angle)), 0},
            { 0, -SDL_sinf(DEGREES_TO_RADIANS(camera_angle)), SDL_cosf(DEGREES_TO_RADIANS(camera_angle)), 0},
            {0,  16, 7, 1},
        }
    };
    RenderSetCamera(&camera);

    Vec2 ball_direction = {
        .x = 0.707f,
        .z = 0.707f,
    };

    SDL_bool quit = SDL_FALSE;
    while (quit == SDL_FALSE) {
        // Input.
        //

        quit = InputProcess(player);


        // Update game.
        //
 
        GameUpdate(objects, sizeof(objects) / sizeof(GameObject), ball, &ball_direction);

        // Render.
        //

        RenderBackground(RGBA_COLOR_GUNMETAL);

        for (size_t i = 0; i < sizeof(objects) / sizeof(GameObject); ++i)
            RenderGameObjectWireframe(&objects[i], objects[i].mesh->color);

        SDL_RenderPresent(renderer);

        SDL_Delay((Uint32)(1.0f / TARGET_FPS * 1000.0f));
    }

Exit:
    // Clean up.
    //

    for (size_t i = 0; i < sizeof(meshes) / sizeof(Mesh*); ++i)
        if (meshes[i] != NULL)
            MeshFree(meshes[i]);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL)   SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
