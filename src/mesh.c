#include "mesh.h"

#include <SDL.h>

#include <stdint.h>
#include <stdio.h>

Mesh* MeshLoadFromObj(const char* filename) {
    Mesh* mesh = NULL;

    FILE* file = NULL;
    if (fopen_s(&file, filename, "r") != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Unable to open '%s'.\n", filename);
        goto Exit;
    }

    mesh = SDL_calloc(1, sizeof(Mesh));
    if (mesh == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Memory allocation for '%s' failure.\n", filename);
        goto Exit;
    }

    uint_fast32_t vertex_count = 0;
    uint_fast32_t face_count = 0;

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v') {
            do ++vertex_count;
            while (fgets(line, sizeof(line), file) && line[0] == 'v');
        }
        if (line[0] == 'f') {
            do ++face_count;
            while (fgets(line, sizeof(line), file) && line[0] == 'f');
        }
    }

    mesh->vertices = SDL_malloc((size_t)vertex_count * sizeof(Vec3));
    mesh->indices = SDL_malloc((size_t)face_count * 3 * sizeof(uint_fast32_t));
    mesh->number_of_faces = face_count;
    mesh->number_of_vertices = vertex_count;

    if (mesh->vertices == NULL || mesh->indices == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Memory allocation failed for `%s`\n", filename);
        MeshFree(mesh);
    }

    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v') {
            for (uint_fast32_t i = 0; i < vertex_count; ++i) {
                sscanf_s(line, "v %f %f %f",
                    &mesh->vertices[i].x, &mesh->vertices[i].y, &mesh->vertices[i].z);
                fgets(line, sizeof(line), file);
            }
        }
        if (line[0] == 'f') {
            for (uint_fast32_t i = 0; i < face_count; ++i) {
                sscanf_s(line, "f %u %u %u",
                    &mesh->indices[(i * 3) + 0], &mesh->indices[(i * 3) + 1], &mesh->indices[(i * 3) + 2]);
                mesh->indices[(i * 3) + 0] -= 1;
                mesh->indices[(i * 3) + 1] -= 1;
                mesh->indices[(i * 3) + 2] -= 1;
                fgets(line, sizeof(line), file);
            }
        }
    }

Exit:
    if (file != NULL) fclose(file);
    return mesh;
}

void MeshFree(Mesh* mesh) {
    if (mesh != NULL) {
        if (mesh->vertices != NULL) SDL_free(mesh->vertices);
        if (mesh->indices != NULL) SDL_free(mesh->indices);
    }
    SDL_free(mesh);
}
