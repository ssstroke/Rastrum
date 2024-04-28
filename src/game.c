#include "game.h"

#include "main.h"

SDL_bool IntersectRaySegmentSphere(const Vec3* o, const Vec3* d, const Vec3* so, float radius2);

// TODO: See if I still need to check for vertices intersections.
void GameUpdate(GameObject* objects, const size_t objects_count, GameObject* ball, Vec2* direction) {
    static uint_fast8_t destroyed_counter = 0;

    for (size_t i = 0; i < objects_count; ++i) {

        // Skip unactive objects and the ball itself.
        if (objects[i].active == SDL_FALSE || &objects[i] == ball)
            continue;

        GameObject* current = &objects[i];

        const Vec3 ball_origin = {
            .x = ball->transform->m[3][0],
            .y = ball->transform->m[3][1],
            .z = ball->transform->m[3][2],
        };
        const float ball_radius = 1.0f;

        for (uint_fast32_t j = 0; j < current->mesh->number_of_faces; ++j) {
            SDL_bool outside_plane    = SDL_FALSE;
            SDL_bool outside_vertices = SDL_FALSE;
            SDL_bool outside_edges    = SDL_FALSE;

            const Vec3 a = Vec3MulByMat4x4(&current->mesh->vertices[current->mesh->indices[j * 3 + 0]], current->transform);
            const Vec3 b = Vec3MulByMat4x4(&current->mesh->vertices[current->mesh->indices[j * 3 + 1]], current->transform);
            const Vec3 c = Vec3MulByMat4x4(&current->mesh->vertices[current->mesh->indices[j * 3 + 2]], current->transform);

            // Check for plane intersection.
            //

            const Vec3 u = Vec3Sub(&b, &a);
            const Vec3 v = Vec3Sub(&c, &a);
            Vec3 abc_normal = Vec3Cross(&u, &v);
            abc_normal = Vec3Normalize(&abc_normal);

            if (SDL_fabsf(abc_normal.y) > 0.1f)
                continue;

            Vec3 abc_average = Vec3Add(&a, &b);
            abc_average = Vec3Add(&abc_average, &c);
            abc_average = Vec3ScalarMul(&abc_average, -(1.0f / 3.0f));

            const float d = Vec3Dot(&abc_average, &abc_normal);

            const float point_to_plane_distance = Vec3Dot(&abc_normal, &ball_origin) + d;

            if (SDL_fabsf(point_to_plane_distance) > ball_radius)
                continue;

            // Check for vertices intersection.
            //

            const Vec3 a_to_ball_origin = Vec3Sub(&a, &ball_origin);
            const SDL_bool outside_a = Vec3Dot(&a_to_ball_origin, &a_to_ball_origin) > ball_radius;
            const Vec3 b_to_ball_origin = Vec3Sub(&b, &ball_origin);
            const SDL_bool outside_b = Vec3Dot(&b_to_ball_origin, &b_to_ball_origin) > ball_radius;
            const Vec3 c_to_ball_origin = Vec3Sub(&c, &ball_origin);
            const SDL_bool outside_c = Vec3Dot(&c_to_ball_origin, &c_to_ball_origin) > ball_radius;

            if (outside_a && outside_b && outside_c)
                outside_vertices = SDL_TRUE;

            // Check for edges intersections.
            //

            const Vec3 bma = Vec3Sub(&b, &a);
            const Vec3 cmb = Vec3Sub(&c, &b);
            const Vec3 amc = Vec3Sub(&a, &c);

            Vec3 ip;

            if (!IntersectRaySegmentSphere(&a, &bma, &ball_origin, ball_radius) &&
                !IntersectRaySegmentSphere(&b, &cmb, &ball_origin, ball_radius) &&
                !IntersectRaySegmentSphere(&c, &amc, &ball_origin, ball_radius))
                outside_edges = SDL_TRUE;

            if (outside_edges && outside_vertices)
                continue;

            // If we are still here, there is a collision. Calculate new direction for the ball.
            //

            const Vec2 mesh_center_to_ball = {
                .x = current->transform->m[3][0] - ball->transform->m[3][0],
                .z = current->transform->m[3][2] - ball->transform->m[3][2],
            };
            const Vec2 abc_normal_v2 = {
                .x = abc_normal.x,
                .z = abc_normal.z,
            };

            const Vec2 unit_x = {
                .x = 1.0f,
                .z = 0.0f,
            };

            // If we hit plane that points left (right), simply inverse x direction.
            //

            if (SDL_fabsf(Vec2Dot(&abc_normal_v2, &unit_x)) == 1) {
                direction->x = -direction->x;
            }
            else {
                // We hit plane that points forward (back), calculate new direction.
                //

                const float hit_angle_cos = Vec2Dot(&mesh_center_to_ball, &abc_normal_v2) / Vec2Length(&mesh_center_to_ball);
                const float hit_angle_sin = SDL_sqrtf(1.0f - hit_angle_cos * hit_angle_cos);

                direction->x = (direction->x * hit_angle_cos) - (direction->z * hit_angle_sin);
                direction->z = (direction->z * hit_angle_cos) + (direction->x * hit_angle_sin);

                if (Vec2Dot(&mesh_center_to_ball, &unit_x) / Vec2Length(&mesh_center_to_ball) < 0)
                    direction->x = -direction->x;
            }

            *direction = Vec2Normalize(direction);

            if (current->entity == SDL_TRUE) {
                current->active = SDL_FALSE;

                // Reset game if all entities have been destroyed.

                destroyed_counter += 1;
                if (destroyed_counter == 8) {
                    destroyed_counter = 0;
                    for (size_t k = 0; k < objects_count; ++k) {
                        objects[k].active = SDL_TRUE;
                        direction->x = 0.0f;
                        direction->z = 0.5f;
                        ball->transform->m[3][0] = 0.0f;
                        ball->transform->m[3][2] = -7.0f;
                    }
                }
            }

            break;
        }
    }

    // Move ball.
    //

    ball->transform->m[3][0] += direction->x * SPEED;
    ball->transform->m[3][2] += direction->z * SPEED;
}

void GameObjectSetTransformPosition(GameObject* object, const float x, const float y, const float z) {
    *object->transform = InitIdentityMatrix();
    object->transform->m[3][0] = x;
    object->transform->m[3][1] = y;
    object->transform->m[3][2] = z;
}

// https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
// TODO: Move it to some other file.
SDL_bool IntersectRaySegmentSphere(const Vec3* o, const Vec3* d, const Vec3* so, float radius2)
{
    const Vec3 d_normalized = Vec3Normalize(d);

    const Vec3 m = Vec3Sub(o, so);
    const float b = Vec3Dot(&m, &d_normalized);
    const float c = Vec3Dot(&m, &m) - radius2;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return SDL_FALSE;
    const float discr = b * b - c;

    if (discr < 0.0f)
        return SDL_FALSE;

    float t = -b - SDL_sqrtf(discr);
    t = SDL_max(0.0f, t);

    const Vec3 _d = Vec3ScalarMul(&d_normalized, t);

    if (t > Vec3Length(d))
        return SDL_FALSE;

    return SDL_TRUE;
}
