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

static inline void *umalloc(size_t sz) {
    void *p = malloc(sz);
    ASSERT(p);
    return p;
}

typedef struct {
    float time;
    int count;
    int capacity;
    pvec_t *f; // force
    pvec_t *x; // position
    pvec_t *v; // velocity
    float *q; // charge
} pp_t;
pp_t pp_new(int capacity) {
    pp_t state;
    state.time = 0.0;
    state.count = 0;
    state.capacity = capacity;
    state.f = umalloc(capacity*sizeof(*state.f));
    state.x = umalloc(capacity*sizeof(*state.x));
    state.v = umalloc(capacity*sizeof(*state.v));
    state.q = umalloc(capacity*sizeof(*state.q));
    return state;
}
void pp_delete(pp_t *state) {
    ASSERT(state);
    free(state->f);
    free(state->x);
    free(state->v);
    free(state->q);
    memset(state, 0, sizeof(*state));
}
void pp_add(pp_t *state, pvec_t x0, pvec_t v0, float q) {
    if (state->count >= state->capacity) return;
    state->x[state->count] = x0;
    state->v[state->count] = v0;
    state->q[state->count] = q;
    state->count++;
}
void pp_reset(pp_t *state) {
    memset(state->f, 0, state->capacity*sizeof(*state->f));
    memset(state->x, 0, state->capacity*sizeof(*state->x));
    memset(state->v, 0, state->capacity*sizeof(*state->v));
    memset(state->q, 0, state->capacity*sizeof(*state->q));
    state->count = 0;
    state->time = 0.0;
}
void pp_draw(pp_t *state) {
    for (int i = 0; i < state->count; i++) {
        DrawCircleV(*(Vector2 *)&state->x[i], 4, (state->q[i] > 0) ? PURPLE : SKYBLUE);
    }
}
void pp_step(pp_t *state, float dt) {
    // clear force accumulators
    memset(state->f, 0, sizeof(*state->f) * state->count);

    // accumulate forces
    for (int i = 0; i < state->count; i++) {
        for (int j = i + 1; j < state->count; j++) {
            if (i == j) continue;
            pvec_t dx = pvec_sub(state->x[i], state->x[j]);
            pvec_t u = pvec_normalize(dx);
            float c = ke * (state->q[i] * state->q[j]) / pvec_mag(dx);
            pvec_accum(&state->f[i], pvec_scale(u, c));
            pvec_accum(&state->f[j], pvec_scale(u, -c));
        }
    }

    // integrate equations of motion
    for (int i = 0; i < state->count; i++) {
        float invm = 1.0 / 10e-3; // mass 10e-3kg
        pvec_accum(&state->x[i], pvec_scale(state->v[i], dt));
        pvec_accum(&state->v[i], pvec_scale(state->f[i], invm * dt));
    }
}
void pp_randomize(pp_t *state) {
    state->count = state->capacity;
    for (int i = 0; i < state->count; i++) {
        state->x[i].x = ((float)rand() / (float)RAND_MAX) * SCW;
        state->x[i].y = ((float)rand() / (float)RAND_MAX) * SCH;
        state->q[i] = ((float)rand() / (float)RAND_MAX) > 0.5 ? CHARGE : -CHARGE;
    }
}


int
main(int argc, char *argv[])
{
    pp_t state = pp_new(1000);
    pp_randomize(&state);

    InitWindow(SCW, SCH, "Basic Particle-Particle Model");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {

        pp_step(&state, GetFrameTime());

        if (IsKeyPressed(KEY_R)) {
            pp_reset(&state);
            pp_randomize(&state);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        pp_draw(&state);

        EndDrawing();
    }

    pp_delete(&state);

    return 0;
}
