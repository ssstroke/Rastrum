#pragma once

#include "algebra.h"
#include "mesh.h"

#include <SDL.h>

void    RenderInitDefaultCamera(void);
Mat4x4  RenderGetCamera(void);
void    RenderSetCamera(Mat4x4* camera);

void RenderSetRenderer(const SDL_Renderer* renderer);

void RenderBackgroud(const int r, const int g, const int b, const int a);
void RenderMeshWireframe(const Mesh* mesh, const int r, const int g, const int b, const int a);
