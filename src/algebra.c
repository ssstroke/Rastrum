#include "algebra.h"

#include <SDL.h>

#include <stdint.h>

Vec3  Vec3ScalarAdd(const Vec3* a, const float b) {
    return (Vec3) {
        .x = a->x + b,
        .y = a->y + b,
        .z = a->z + b,
    };
}

Vec3  Vec3ScalarMul(const Vec3* a, const float b) {
    return (Vec3) {
        .x = a->x * b,
        .y = a->y * b,
        .z = a->z * b,
    };
}

Vec3 Vec3Add(const Vec3* a, const Vec3* b) {
    return (Vec3) {
        .x = a->x + b->x,
        .y = a->y + b->y,
        .z = a->z + b->z,
    };
}

Vec3 Vec3Sub(const Vec3* a, const Vec3* b) {
    return (Vec3) {
        .x = a->x - b->x,
        .y = a->y - b->y,
        .z = a->z - b->z,
    };
}

float Vec3Dot(const Vec3* a, const Vec3* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

Vec3 Vec3Cross(const Vec3* a, const Vec3* b) {
    return (Vec3) {
        .x = a->y * b->z - a->z * b->y,
        .y = a->z * b->x - a->x * b->z,
        .z = a->x * b->y - a->y * b->x,
    };
}

float Vec3Length(const Vec3* a) {
    return SDL_sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
}

Vec3 Vec3Normalize(const Vec3* a) {
    const float len_inverse = 1.0f / Vec3Length(a);
    return (Vec3) {
        .x = a->x * len_inverse,
        .y = a->y * len_inverse,
        .z = a->z * len_inverse,
    };
}

Vec3 Vec3MulByMat4x4(const Vec3* a, const Mat4x4* m) {
    Vec3 result = {
        .x = a->x * m->m[0][0] + a->y * m->m[1][0] + a->z * m->m[2][0] + m->m[3][0],
        .y = a->x * m->m[0][1] + a->y * m->m[1][1] + a->z * m->m[2][1] + m->m[3][1],
        .z = a->x * m->m[0][2] + a->y * m->m[1][2] + a->z * m->m[2][2] + m->m[3][2],
    };

    const float w_inverse = 1.0f / (a->x * m->m[0][3] + a->y * m->m[1][3] + a->z * m->m[2][3] + m->m[3][3]);
    if (w_inverse != 1.0f && w_inverse != 0.0f) {
        result.x *= w_inverse;
        result.y *= w_inverse;
        result.z *= w_inverse;
    }

    return result;
}

Mat4x4 InitIdentityMatrix(void) {
    return (Mat4x4) {
        .m = {
            {1,  0,  0,  0},
            {0,  1,  0,  0},
            {0,  0,  1,  0},
            {0,  0,  0,  1},
        }
    };
}

Mat4x4 Mat4x4Mul(const Mat4x4* a, const Mat4x4* b) {
    Mat4x4 result;
    for (uint_fast8_t row = 0; row < 4; ++row) {
        for (uint_fast8_t col = 0; col < 4; ++col) {
            result.m[row][col] =
                a->m[row][0] * b->m[0][col] +
                a->m[row][1] * b->m[1][col] +
                a->m[row][2] * b->m[2][col] +
                a->m[row][3] * b->m[3][col];
        }
    }
    return result;
}

Mat4x4 Mat4x4Inverse(const Mat4x4* a) {
    Mat4x4 s = {
        .m = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }
    };
    Mat4x4 t = *a;

    // Forward elimination
    for (uint_fast8_t i = 0; i < 3; i++) {
        // Step 1: Choose a pivot
        uint_fast8_t pivot = i;
        float pivotSize = t.m[i][i];
        if (pivotSize < 0) pivotSize = -pivotSize;

        for (uint_fast8_t j = i + 1; j < 4; j++) {
            float tmp = t.m[j][i];
            if (tmp < 0) tmp = -tmp;

            if (tmp > pivotSize) {
                pivot = j;
                pivotSize = tmp;
            }
        }

        if (pivotSize == 0) return s;

        if (pivot != i) {
            for (uint_fast8_t j = 0; j < 4; j++) {
                float tmp;
                tmp = t.m[i][j];
                t.m[i][j] = t.m[pivot][j];
                t.m[pivot][j] = tmp;
                tmp = s.m[i][j];
                s.m[i][j] = s.m[pivot][j];
                s.m[pivot][j] = tmp;
            }
        }

        // Step 2: Eliminate all the numbers below the diagonal
        for (uint_fast8_t j = i + 1; j < 4; j++) {
            float f = t.m[j][i] / t.m[i][i];
            for (uint_fast8_t k = 0; k < 4; k++) {
                t.m[j][k] -= f * t.m[i][k];
                s.m[j][k] -= f * s.m[i][k];
            }
            // Set the column value to exactly 0 in case
            // numeric round-off left it a very tiny number
            t.m[j][i] = 0.f;
        }
    }

    // Step 3: Set elements along the diagonal to 1.0
    for (uint_fast8_t i = 0; i < 4; i++) {
        float divisor = t.m[i][i];
        for (uint_fast8_t j = 0; j < 4; j++) {
            t.m[i][j] = t.m[i][j] / divisor;
            s.m[i][j] = s.m[i][j] / divisor;
        }
        // Set the diagonal to 1.0 exactly to avoid
        // possible round-off error
        t.m[i][i] = 1.f;
    }

    // Step 4: Eliminate all the numbers above the diagonal
    for (uint_fast8_t i = 0; i < 3; i++) {
        for (uint_fast8_t j = i + 1; j < 4; j++) {
            float constant = t.m[i][j];
            for (uint_fast8_t k = 0; k < 4; k++) {
                t.m[i][k] -= t.m[j][k] * constant;
                s.m[i][k] -= s.m[j][k] * constant;
            }
            t.m[i][j] = 0.f; // In case of round-off error
        }
    }

    return s;
}
