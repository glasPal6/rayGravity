#include <stdint.h>

// #include "clay.h"
#include "raylib.h"
#include "raymath.h"

#define N_Obj 2
#define GRAVITY_CONST 200.0
// #define dt GetFrameTime()
#define dt 0.1

typedef struct {
    Vector2 pos;
    Vector2 vel;
    int16_t mass;
} Object;

void update_objects(Object* objects) {
    // Calculate all accelerations first
    Vector2 accelerations[N_Obj] = {0};

    for (int i = 0; i < N_Obj; i++) {
        for (int j = i + 1; j < N_Obj; j++) {
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
            accelerations[j] = Vector2Subtract(accelerations[j], acc);
        }
    }

    // Apply accelerations and update positions
    for (int i = 0; i < N_Obj; i++) {
        objects[i].vel =
            Vector2Add(objects[i].vel, Vector2Scale(accelerations[i], dt));
        objects[i].pos =
            Vector2Add(objects[i].pos, Vector2Scale(objects[i].vel, dt));
    }
}

int main() {
    InitWindow(1600, 900, "Gravity Simulator");

    Object objects[] = {
        {
            .pos = (Vector2){200.0f, 200.0f},
            .vel = (Vector2){-5.0f, 0.0f},
            .mass = 1000,
        },
        {
            .pos = (Vector2){700.0f, 400.0f},
            .vel = (Vector2){0.0f, 10.0f},
            .mass = 500,
        },
    };

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        update_objects(objects);

        // Draw
        BeginDrawing();
        DrawFPS(10, 10);
        ClearBackground(RAYWHITE);

        for (int i = 0; i < N_Obj; i++) {
            DrawCircleV(objects[i].pos, 1, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
