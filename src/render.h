#pragma once

#include "algebra.h"
#include "game.h"
#include "mesh.h"

#include <SDL.h>

void    RenderInitDefaultCamera(void);
Mat4x4  RenderGetCamera(void);
void    RenderSetCamera(const Mat4x4* camera);
void    RenderInitCamera(
    const float x, const float y, const float z,
    const float angle_x, const float angle_y, const float angle_z);

void RenderSetRenderer(const SDL_Renderer* renderer);

void RenderBackground(const Uint32 color);
void RenderGameObjectWireframe(const GameObject* object, const Uint32 color);
