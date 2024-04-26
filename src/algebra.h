#pragma once

#include <SDL.h>

#define DEGREES_TO_RADIANS(x) (float)x * (float)M_PI / 180.0f
#define RADIANS_TO_DEGREES(x) (float)x * 180.0f / (float)M_PI

typedef struct {
    float x;
    float z;
} Vec2;

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float m[4][4];
} Mat4x4;

// Vec2 functions.
//

Vec2  Vec2ScalarAdd(const Vec2* a, const float b);
Vec2  Vec2ScalarMul(const Vec2* a, const float b);

Vec2  Vec2Add(const Vec2* a, const Vec2* b);
Vec2  Vec2Sub(const Vec2* a, const Vec2* b);
float Vec2Dot(const Vec2* a, const Vec2* b);

float Vec2Length(const Vec2* a);

Vec2  Vec2Normalize(const Vec2* a);

// Vec3 functions.
//

Vec3  Vec3ScalarAdd(const Vec3* a, const float b);
Vec3  Vec3ScalarMul(const Vec3* a, const float b);

Vec3  Vec3Add(const Vec3* a, const Vec3* b);
Vec3  Vec3Sub(const Vec3* a, const Vec3* b);
float Vec3Dot(const Vec3* a, const Vec3* b);
Vec3  Vec3Cross(const Vec3* a, const Vec3* b);

float Vec3Length(const Vec3* a);

Vec3  Vec3Normalize(const Vec3* a);

Vec3  Vec3MulByMat4x4(const Vec3* a, const Mat4x4* m);

// Mat4x4 functions.
//

Mat4x4 InitIdentityMatrix(void);

Mat4x4 Mat4x4Mul(const Mat4x4* a, const Mat4x4* b);
Mat4x4 Mat4x4Inverse(const Mat4x4* a);
