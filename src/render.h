#pragma once

#include "algebra.h"
#include "mesh.h"

#include <SDL.h>

void    RenderInitDefaultCamera(void);
Mat4x4  RenderGetCamera(void);
void    RenderSetCamera(const Mat4x4* camera);

void RenderSetRenderer(const SDL_Renderer* renderer);

void RenderBackgroud(const Uint32 color);
void RenderMeshWireframe(const Mesh* mesh, const Uint32 color);
