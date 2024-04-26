#pragma once

#include "algebra.h"

#include <stdint.h>

// All of the faces are nothing but triangles.
typedef struct {
    Mat4x4* transform;
    Vec3* vertices;
    uint_fast32_t* indices;
    uint_fast32_t number_of_faces;
    uint_fast32_t number_of_vertices;
    Uint32 color;
} Mesh;

Mesh* MeshLoadFromObj(const char* filename);
void  MeshFree(Mesh* mesh);
