#include "render.h"

#include "main.h"

#include <SDL.h>

SDL_Renderer* g_renderer = NULL;
Mat4x4        g_camera;
Mat4x4        g_world_to_camera;

void RenderInitDefaultCamera(void) {
    g_camera = InitIdentityMatrix();
    g_world_to_camera = Mat4x4Inverse(&g_camera);
}

Mat4x4 RenderGetCamera() {
    return g_camera;
}

void RenderSetCamera(Mat4x4* camera) {
    g_camera = *camera;
    g_world_to_camera = Mat4x4Inverse(&g_camera);
}

void RenderSetRenderer(const SDL_Renderer* renderer) {
    g_renderer = renderer;
}

void RenderBackgroud(const int r, const int g, const int b, const int a) {
    SDL_SetRenderDrawColor(g_renderer, r, g, b, a);
    SDL_RenderClear(g_renderer);
}

void RenderMeshWireframe(const Mesh* mesh, const int r, const int g, const int b, const int a) {
    // Iterate over every face.
    //

    for (uint_fast32_t i = 0; i < mesh->number_of_faces; ++i) {
        // Face vertices (in world space).
        //

        const Vec3 a_world = Vec3MulByMat4x4(&mesh->vertices[mesh->indices[i * 3 + 0]], mesh->transform);
        const Vec3 b_world = Vec3MulByMat4x4(&mesh->vertices[mesh->indices[i * 3 + 1]], mesh->transform);
        const Vec3 c_world = Vec3MulByMat4x4(&mesh->vertices[mesh->indices[i * 3 + 2]], mesh->transform);

        // Face vertices (in camera space).
        //

        const Vec3 a_camera = Vec3MulByMat4x4(&a_world, &g_world_to_camera);
        const Vec3 b_camera = Vec3MulByMat4x4(&b_world, &g_world_to_camera);
        const Vec3 c_camera = Vec3MulByMat4x4(&c_world, &g_world_to_camera);

        // Projected vertices (in screen space).
        // Ignore z-value for now.
        //
        
        const Vec3 a_projected = {
            .x = a_camera.x / ((a_camera.z == 0.0f) ? 1.0f : -a_camera.z),
            .y = a_camera.y / ((a_camera.z == 0.0f) ? 1.0f : -a_camera.z),
            .z = a_camera.z,
        };
        const Vec3 b_projected = {
            .x = b_camera.x / ((b_camera.z == 0.0f) ? 1.0f : -b_camera.z),
            .y = b_camera.y / ((b_camera.z == 0.0f) ? 1.0f : -b_camera.z),
            .z = b_camera.z,
        };
        const Vec3 c_projected = {
            .x = c_camera.x / ((c_camera.z == 0.0f) ? 1.0f : -c_camera.z),
            .y = c_camera.y / ((c_camera.z == 0.0f) ? 1.0f : -c_camera.z),
            .z = c_camera.z,
        };

        // TODO: Check if vertices are inside screen.
        // ...
        //

        // NDC coordinates.
        //

        const Vec3 a_normalized = {
            .x = (a_projected.x + 0.5f) / 1.0f,
            .y = (a_projected.y + 0.5f) / 1.0f,
        };
        const Vec3 b_normalized = {
            .x = (b_projected.x + 0.5f) / 1.0f,
            .y = (b_projected.y + 0.5f) / 1.0f,
        };
        const Vec3 c_normalized = {
            .x = (c_projected.x + 0.5f) / 1.0f,
            .y = (c_projected.y + 0.5f) / 1.0f,
        };

        // Raster space.
        //

        const int a_raster_x = (int)SDL_floorf(a_normalized.x * WINDOW_WIDTH);
        const int a_raster_y = (int)SDL_floorf((1.0f - a_normalized.y) * WINDOW_HEIGHT);
        const int b_raster_x = (int)SDL_floorf(b_normalized.x * WINDOW_WIDTH);
        const int b_raster_y = (int)SDL_floorf((1.0f - b_normalized.y) * WINDOW_HEIGHT);
        const int c_raster_x = (int)SDL_floorf(c_normalized.x * WINDOW_WIDTH);
        const int c_raster_y = (int)SDL_floorf((1.0f - c_normalized.y) * WINDOW_HEIGHT);

        // Draw lines.
        //

        const SDL_Point points[] = {
            {.x = a_raster_x, .y = a_raster_y}, // 1
            {.x = b_raster_x, .y = b_raster_y}, // 2
            {.x = c_raster_x, .y = c_raster_y}, // 3
            {.x = a_raster_x, .y = a_raster_y}, // 4
        };
        SDL_SetRenderDrawColor(g_renderer, r, g, b, a);
        SDL_RenderDrawLines(g_renderer, points, 4);
    };
}
