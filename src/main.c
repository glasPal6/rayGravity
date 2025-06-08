#include <stdint.h>

// #include "clay.h"
#include "raylib.h"
#include "raymath.h"

#define N_Obj 2
#define GRAVITY_CONST 250.0

typedef struct {
    Vector2 pos;
    Vector2 vel;
    int16_t mass;
    int16_t radius;
    Color color;
} Object;

void update_objects(Object* objects, float dt) {
    // Calculate all accelerations first
    Vector2 accelerations[N_Obj] = {0};

    for (int i = 0; i < N_Obj; i++) {
        for (int j = 0; j < N_Obj; j++) {
            if (j == i) continue;  // Skip self-interaction

            Vector2 r = Vector2Subtract(objects[j].pos, objects[i].pos);
            float distance = Vector2Length(r);

            // Avoid division by very small numbers
            if (distance < 0.1f) continue;

            Vector2 r_n = Vector2Normalize(r);

            // Calculate gravitational acceleration: G * m / r²
            Vector2 acc = Vector2Scale(
                r_n, GRAVITY_CONST * objects[j].mass / (distance * distance));

            accelerations[i] = Vector2Add(accelerations[i], acc);
        }
    }

    // Apply accelerations and update positions
    for (int i = 0; i < N_Obj; i++) {
        // Update velocity based on acceleration
        objects[i].vel =
            Vector2Add(objects[i].vel, Vector2Scale(accelerations[i], dt));

        // Update position based on velocity
        objects[i].pos =
            Vector2Add(objects[i].pos, Vector2Scale(objects[i].vel, dt));
    }
}

int main() {
    InitWindow(800, 450, "Gravity Simulator");

    Object objects[] = {
        {
            .pos = (Vector2){200.0f, 200.0f},
            .vel = (Vector2){0.0f, 0.0f},
            .mass = 1000,
            .radius = 10.0f,
            .color = BLUE,
        },
        {
            .pos = (Vector2){700.0f, 400.0f},
            .vel = (Vector2){0.0f, -10.0f},
            .mass = 500,
            .radius = 5.0f,
            .color = RED,
        },
    };

    while (!WindowShouldClose()) {
        // Update
        float dt = GetFrameTime();
        update_objects(objects, dt);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < N_Obj; i++) {
            DrawCircleV(objects[i].pos, objects[i].radius, objects[i].color);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
