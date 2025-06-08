#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "raymath.h"

#define WIDTH 1600
#define HEIGHT 900

#define N_Obj 50
#define GRAVITY_CONST 200.0 / N_Obj
#define dt 0.1
#define BOUNCE_FACTOR 0.5
#define MAX_VELOCITY 200
#define RADIUS 5
#define TRAIL_COUNT 100
#define TRAIL_FADE 0.8f

typedef struct {
    Vector2 pos;
    Vector2 vel;
    int16_t mass;
    Vector2 trail[TRAIL_COUNT];
    int trailCount;
} Object;

void randomize_objects(Object* objects, int count, int width, int height) {
    for (int i = 0; i < count; i++) {
        // Random position within screen bounds
        objects[i].pos.x = (float)(rand() % (width - 100) + 50);
        objects[i].pos.y = (float)(rand() % (height - 100) + 50);

        // Random velocity (-20 to +20)
        objects[i].vel.x = (float)((rand() % 21) - 10);
        objects[i].vel.y = (float)((rand() % 21) - 10);

        // Random mass (100 to 2000)
        objects[i].mass = (int16_t)(rand() % 1901 + 100);

        // Trail
        for (int j = 0; j < TRAIL_COUNT; j++) {
            objects[i].trail[j] = objects[i].pos;
        }
    }
}

void update_objects(Object* objects) {
    // Calculate all accelerations first
    Vector2 accelerations[N_Obj] = {0};

    for (int i = 0; i < N_Obj; i++) {
        for (int j = i + 1; j < N_Obj; j++) {
            Vector2 r = Vector2Subtract(objects[j].pos, objects[i].pos);
            float distance = Vector2Length(r);

            // Avoid division by very small numbers
            if (distance < 1.2 * RADIUS) continue;

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
        // Update trail (shift all positions)
        for (int j = TRAIL_COUNT - 1; j > 0; j--) {
            objects[i].trail[j] = objects[i].trail[j - 1];
        }
        objects[i].trail[0] = objects[i].pos;
        if (objects[i].trailCount < TRAIL_COUNT) objects[i].trailCount++;

        objects[i].vel =
            Vector2Add(objects[i].vel, Vector2Scale(accelerations[i], dt));
        objects[i].vel =
            Vector2ClampValue(objects[i].vel, -1 * MAX_VELOCITY, MAX_VELOCITY);
        objects[i].pos =
            Vector2Add(objects[i].pos, Vector2Scale(objects[i].vel, dt));

        // Right wall
        if (objects[i].pos.x + RADIUS > WIDTH) {
            objects[i].pos.x = WIDTH - RADIUS;
            objects[i].vel.x = -objects[i].vel.x * BOUNCE_FACTOR;
        }
        // Left wall
        if (objects[i].pos.x - RADIUS < 0) {
            objects[i].pos.x = RADIUS;
            objects[i].vel.x = -objects[i].vel.x * BOUNCE_FACTOR;
        }
        // Bottom wall
        if (objects[i].pos.y + RADIUS > HEIGHT) {
            objects[i].pos.y = HEIGHT - RADIUS;
            objects[i].vel.y = -objects[i].vel.y * BOUNCE_FACTOR;
        }
        // Top wall
        if (objects[i].pos.y - RADIUS < 0) {
            objects[i].pos.y = RADIUS;
            objects[i].vel.y = -objects[i].vel.y * BOUNCE_FACTOR;
        }
    }
}

int main() {
    const int screenWidth = WIDTH;
    const int screenHeight = HEIGHT;

    InitWindow(screenWidth, screenHeight, "Gravity Simulator");

    // srand((unsigned int)time(NULL));
    srand(0);
    Object objects[N_Obj];
    randomize_objects(objects, N_Obj, screenWidth, screenHeight);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update
        update_objects(objects);

        // Draw
        BeginDrawing();
        DrawFPS(10, 10);
        ClearBackground(RAYWHITE);

        for (int i = 0; i < N_Obj; i++) {
            for (int j = 0; j < TRAIL_COUNT; j++) {
                // Calculate alpha based on position in trail (newer positions =
                // more opaque)
                float alpha = (1.0f - (float)j / TRAIL_COUNT) * TRAIL_FADE;
                float trailRadius =
                    RADIUS *
                    (0.2f + 0.8f * alpha);  // Trail gets smaller as it fades

                DrawCircleV(objects[i].trail[j], trailRadius,
                            ColorAlpha(BLACK, alpha));
            }

            // Draw the current object
            DrawCircleV(objects[i].pos, RADIUS, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
