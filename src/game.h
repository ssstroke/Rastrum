#pragma once

#include "mesh.h"

#define SPEED 0.075f

typedef struct {
    SDL_bool active;
    SDL_bool entity;
    Mesh* mesh;
    Mat4x4* transform;
} GameObject;

void GameUpdate(const GameObject* objects, const size_t objects_count, GameObject* player, Vec2* direction);
void GameObjectSetTransformPosition(GameObject* object, const float x, const float y, const float z);
