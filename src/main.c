#include "main.h"

// This project headers.
#include "algebra.h"
#include "mesh.h"
#include "render.h"

// Third-party headers.
#include <SDL.h>

// Standard headers.

#define RGBA_COLOR_BLACK    0x00, 0x00, 0x00, 0xff
#define RGBA_COLOR_WHITE    0xff, 0xff, 0xff, 0xff

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    Mesh* mesh = NULL;

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

        SDL_assert(window != NULL);
        SDL_assert(renderer != NULL);
    }

    mesh = MeshLoadFromObj("assets/monkey_solid_2.obj");
    SDL_assert(mesh != NULL);

    RenderInitDefaultCamera();
    RenderSetRenderer(renderer);

    float angle_degrees_x = 1.0f;
    float angle_degrees_y = 1.0f;

    SDL_bool quit = SDL_FALSE;
    while (quit == SDL_FALSE) {
        SDL_Event event;
        while (SDL_PollEvent(&event) == 1) {
            if (event.type == SDL_QUIT) {
                quit = SDL_TRUE;
                break;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP) {
                    Mat4x4 camera = RenderGetCamera();
                    camera.m[3][1] += 1.0f;
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_LEFT) {
                    Mat4x4 camera = RenderGetCamera();
                    camera.m[3][0] -= 1.0f;
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    Mat4x4 camera = RenderGetCamera();
                    camera.m[3][1] -= 1.0f;
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    Mat4x4 camera = RenderGetCamera();
                    camera.m[3][0] += 1.0f;
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_w) { // Rotate X
                    Mat4x4 camera = RenderGetCamera();
                    Mat4x4 rotation_x = {
                        .m = {
                            { 1, 0, 0, 0 },
                            { 0,  SDL_cosf(DEGREES_TO_RADIANS(10.0f)), SDL_sinf(DEGREES_TO_RADIANS(10.0f)), 0},
                            { 0, -SDL_sinf(DEGREES_TO_RADIANS(10.0f)), SDL_cosf(DEGREES_TO_RADIANS(10.0f)), 0},
                            { 0, 0, 0, 1 },
                        }
                    };
                    camera = Mat4x4Mul(&camera, &rotation_x);
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_s) { // Rotate X
                    Mat4x4 camera = RenderGetCamera();
                    Mat4x4 rotation_x = {
                        .m = {
                            { 1, 0, 0, 0 },
                            { 0,  SDL_cosf(DEGREES_TO_RADIANS(-10.0f)), SDL_sinf(DEGREES_TO_RADIANS(-10.0f)), 0},
                            { 0, -SDL_sinf(DEGREES_TO_RADIANS(-10.0f)), SDL_cosf(DEGREES_TO_RADIANS(-10.0f)), 0},
                            { 0, 0, 0, 1 },
                        }
                    };
                    camera = Mat4x4Mul(&camera, &rotation_x);
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_a) { // Rotate Y
                    Mat4x4 camera = RenderGetCamera();
                    Mat4x4 rotation_y = {
                        .m = {
                            { SDL_cosf(DEGREES_TO_RADIANS(10.0f)), 0, -SDL_sinf(DEGREES_TO_RADIANS(10.0f)), 0 },
                            { 0, 1, 0, 0 },
                            { SDL_sinf(DEGREES_TO_RADIANS(10.0f)), 0, SDL_cosf(DEGREES_TO_RADIANS(10.0f)), 0 },
                            { 0, 0, 0, 1 },
                        }
                    };
                    camera = Mat4x4Mul(&camera, &rotation_y);
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_d) { // Rotate Y
                    Mat4x4 camera = RenderGetCamera();
                    Mat4x4 rotation_y = {
                        .m = {
                            { SDL_cosf(DEGREES_TO_RADIANS(-10.0f)), 0, -SDL_sinf(DEGREES_TO_RADIANS(-10.0f)), 0 },
                            { 0, 1, 0, 0 },
                            { SDL_sinf(DEGREES_TO_RADIANS(-10.0f)), 0, SDL_cosf(DEGREES_TO_RADIANS(-10.0f)), 0 },
                            { 0, 0, 0, 1 },
                        }
                    };
                    camera = Mat4x4Mul(&camera, &rotation_y);
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_f) {
                    Mat4x4 camera = RenderGetCamera();
                    camera.m[3][2] -= 1.0f;
                    RenderSetCamera(&camera);
                }
                else if (event.key.keysym.sym == SDLK_v) {
                    Mat4x4 camera = RenderGetCamera();
                    camera.m[3][2] += 1.0f;
                    RenderSetCamera(&camera);
                }
            }
        }

        {
            Mat4x4 rotation_x = {
            .m = {
                { 1, 0, 0, 0 },
                { 0,  SDL_cosf(DEGREES_TO_RADIANS(angle_degrees_x)), SDL_sinf(DEGREES_TO_RADIANS(angle_degrees_x)), 0},
                { 0, -SDL_sinf(DEGREES_TO_RADIANS(angle_degrees_x)), SDL_cosf(DEGREES_TO_RADIANS(angle_degrees_x)), 0},
                { 0, 0, 0, 1 },
            }
            };
            Mat4x4 rotation_y = {
                .m = {
                    { SDL_cosf(DEGREES_TO_RADIANS(angle_degrees_y)), 0, -SDL_sinf(DEGREES_TO_RADIANS(angle_degrees_y)), 0 },
                    { 0, 1, 0, 0 },
                    { SDL_sinf(DEGREES_TO_RADIANS(angle_degrees_y)), 0, SDL_cosf(DEGREES_TO_RADIANS(angle_degrees_y)), 0 },
                    { 0, 0, 0, 1 },
                }
            };
            Mat4x4 rotation_xy = Mat4x4Mul(&rotation_x, &rotation_y);
            rotation_xy.m[3][2] = -4;
            mesh->transform = &rotation_xy;

            angle_degrees_x += 0.2f;
            angle_degrees_y += 0.4f;
        }

        RenderBackgroud(RGBA_COLOR_BLACK);
        RenderMeshWireframe(mesh, RGBA_COLOR_WHITE);

        SDL_RenderPresent(renderer);
    }

Exit:
    MeshFree(mesh);
    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL) SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
