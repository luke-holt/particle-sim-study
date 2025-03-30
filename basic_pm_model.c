#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "pvec.h"

#include <assert.h>
#define ASSERT assert

#define ke (8.9875e9) // Coulomb's constant
#define CHARGE (10e-6)

#define SCW (800)
#define SCH (600)
#define FPS (60)
#define GRID (7)

static inline void *umalloc(size_t sz) {
    void *p = malloc(sz);
    ASSERT(p);
    return p;
}

#define PFIELD(state, c, r) ((state)->pfield[(state)->fw*(r)+(c)])
typedef struct {
    float time; // simulation time (s)
    int count; // particle count
    int capacity; // particle capacity
    pvec_t *x; // particle position (m)
    float *q; // particel charge (C)

    int fw; // field width
    int fh; // field height
    float r; // grid spacing
    pvec_t *pfield; // potential field (V/m, or N/C)
} pm_t;
pm_t pm_new(int fw, int fh, float r, int capacity) {
    pm_t state;
    state.time = 0.0;
    state.count = 0;
    state.capacity = capacity;
    state.x = umalloc(sizeof(*state.x)*capacity);
    state.q = umalloc(sizeof(*state.q)*capacity);
    state.fw = fw;
    state.fh = fh;
    state.r = r;
    state.pfield = umalloc(sizeof(*state.pfield)*fw*fh);
    return state;
}
void pm_delete(pm_t *state) {
    ASSERT(state);
    free(state->x);
    free(state->q);
    free(state->pfield);
    memset(state, 0, sizeof(*state));
}
void pm_step(pm_t *state, float detla_time) {
    // clear electric field
    memset(state->pfield, 0, sizeof(*state->pfield)*state->fw*state->fh);

    // potential field
    for (int row = 0; row < state->fh; row++) {
        for (int col = 0; col < state->fw; col++) {
            pvec_t point = (pvec_t){col*state->r, row*state->r};
            pvec_t *pfield = &PFIELD(state, col, row);
            *pfield = (pvec_t){0.0, 0.0};

            // effect of every particle on the mesh
            for (int p = 0; p < state->count; p++) {
                pvec_t r = pvec_sub(point, state->x[p]);
                pvec_t ru = pvec_normalize(r);
                float c = ke * state->q[p] / pvec_magsq(r);
                pvec_accum(pfield, pvec_scale(ru, c));
            }
        }
    }
}
void pm_draw(pm_t *state) {
    // draw field
    for (int row = 0; row < state->fh; row++) {
        for (int col = 0; col < state->fw; col++) {
            pvec_t start = (pvec_t){col*state->r, row*state->r};
            pvec_t f = PFIELD(state, col, row);
            float m = pvec_mag(f);

            // clip to 10
            if (m > 10)
                m = 10;

            pvec_t delta = pvec_scale(pvec_normalize(f), m);
            pvec_t end = pvec_add(start, delta);

            if (m > 2)
                DrawLineV(*(Vector2 *)&start, *(Vector2 *)&end, GRAY);
        }
    }

    // draw particles
    for (int p = 0; p < state->count; p++) {
        DrawCircleV(*(Vector2 *)&state->x[p], 6, state->q[p] > 0 ? PINK : SKYBLUE);
    }
}
void pm_add(pm_t *state, pvec_t x, float q) {
    if (state->count >= state->capacity)
        return;
    state->x[state->count] = x;
    state->q[state->count] = q;
    state->count++;
}
void pm_reset(pm_t *state) {
    memset(state->pfield, 0, sizeof(*state->pfield)*state->fw*state->fh);
    state->count = 0;
}


int
main(int argc, char *argv[])
{
    pm_t state = pm_new(SCW/GRID, SCH/GRID, GRID, 100);

    InitWindow(SCW, SCH, "Basic Particle-Mesh Model");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {

        if (IsKeyPressed(KEY_R)) {
            pm_reset(&state);
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mpos = GetMousePosition();
            pm_add(&state, *(pvec_t *)&mpos, CHARGE);
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mpos = GetMousePosition();
            pm_add(&state, *(pvec_t *)&mpos, -CHARGE);
        }

        pm_step(&state, GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);

        pm_draw(&state);

        EndDrawing();
    }

    pm_delete(&state);

    return 0;
}
