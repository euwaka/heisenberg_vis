#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdbool.h>

#define FPS 60
#define WIDTH 1800
#define HEIGHT 1000
#define NUMBER_PARTICLES 100

#define SLIT_X_POSITION 500
#define SLIT_LENGTH 400

float slit = (float)HEIGHT * 0.1f;
float spawn_time = 0.0f;
float spawn_frequency = 0.3f;

static size_t collapses = 0;

enum phase_t {
    PHASE_BEFORE_SLIT,
    PHASE_INSIDE_SLIT,
    PHASE_AFTER_SLIT,
};

// Contains uncertainty only in y and py
typedef struct particle_t {
    Vector2 position;
    Vector2 momentum;
    float dy;
    float dp_y;
    enum phase_t phase;
    bool active;
} particle_t;

static particle_t particles[NUMBER_PARTICLES] = {0};

void render(void);
void update(float dt);
void processInput(void);

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Heisenberg's Principle Visualization");
    SetTargetFPS(FPS);

    while (!WindowShouldClose()) {
	float dt = GetFrameTime();
	
	processInput();
	update(dt);
	render();
    }

    CloseWindow();
    return 0;
}

float Gaussian2D(float x, float y, float sigma) {
    return expf(-(x * x + y * y) / (2.0f * sigma * sigma)) / (2.0f * PI * sigma * sigma);
}

void spawn_particle(particle_t particles[static 1]) {
    for (size_t i = 0; i < NUMBER_PARTICLES; ++i) {
	particle_t* particle = &particles[i];

	if (!particle->active) {
	    *particle = (particle_t) {
		.position = {0, (float)HEIGHT/2},
		.momentum = {100, 0},
		.dy = 300,
		.dp_y = 30,
		.phase = PHASE_BEFORE_SLIT,
		.active = true,
	    };

	    break;
	}
    }
}

void check_particle_bounds(particle_t* particle) {
    Rectangle slit_rect = {
	SLIT_X_POSITION,
	(HEIGHT - slit) * 0.5,
	SLIT_LENGTH,
	slit
    };
    
    switch (particle->phase) {
    case PHASE_BEFORE_SLIT: {
	bool collision = CheckCollisionPointRec(particle->position, slit_rect);
	
	if (collision && GetRandomValue(0, 10) < 9) {
	    collapses++;
	    particle->phase = PHASE_INSIDE_SLIT;
	    particle->dy = slit / 2.0;
	    particle->dp_y = 500.0;
	} else if (particle->position.x >= SLIT_X_POSITION) {
	    particle->active = false;
	}
	break;
    }
    case PHASE_INSIDE_SLIT: {
	if (particle->position.x >= SLIT_X_POSITION + SLIT_LENGTH) {
	    particle->phase = PHASE_AFTER_SLIT;
	}      
	break;
    }
    case PHASE_AFTER_SLIT:
	if (!CheckCollisionPointRec(particle->position, (Rectangle){ 0, 0, WIDTH, HEIGHT })) {
	    particle->active = false;
	}
    }
}

void render_particle(particle_t* particle) {
    Rectangle position = {
	particle->position.x - 5.0f,
	particle->position.y - particle->dy,
	10.0f,
	2.0 * particle->dy,
    };
    DrawRectangleRounded(position, 5.0f, 4, DARKGRAY);	
}

void render(void) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    // Draw the slit
    DrawRectangle(SLIT_X_POSITION, 0, SLIT_LENGTH, (HEIGHT - slit) * 0.5, DARKGRAY);
    DrawRectangle(SLIT_X_POSITION, (HEIGHT + slit) * 0.5, SLIT_LENGTH, (HEIGHT - slit) * 0.5, DARKGRAY);

    for (size_t i = 0; i < NUMBER_PARTICLES; ++i) {
	particle_t* particle = &particles[i];
	render_particle(particle);
    }

    // Draw text
    char buffer[100];
    sprintf(buffer, "Wavefunctions collapsed: %zu", collapses);
    DrawText(buffer, WIDTH / 2 - 30 * 25 / 2, 20, 25, RED);
    DrawText("delta_x low, delta_p large", WIDTH / 2 - 30 * 25 / 2, 80, 25, RED);
    DrawText("delta_x large, delta_p low", 20, 20, 25, RED);
    DrawText("delta_x increases, delta_p large", WIDTH - 500, 20, 25, RED);
    
    EndDrawing();
}

void update(float dt) {
    // Spawn particles at some frequency
    spawn_time += dt;
    if (spawn_time >= spawn_frequency) {
	spawn_particle(particles);
	spawn_time = 0.0f;
    }

    // Move particles
    for (size_t i = 0; i < NUMBER_PARTICLES; ++i) {
	particle_t* particle = &particles[i];

	particle->momentum.y = GetRandomValue(
	    particle->momentum.y - particle->dp_y,
	    particle->momentum.y + particle->dp_y
	);
	if (particle->momentum.y > particle->dp_y)
	    particle->momentum.y = particle->dp_y;
	if (particle->momentum.y < -particle->dp_y)
	    particle->momentum.y = -particle->dp_y;
	
	particle->position.x += particle->momentum.x * dt;
	
	// wavefunction expands
	if (particle->phase == PHASE_AFTER_SLIT)
	{
	    particle->dy += 0.1f * particle->dp_y * dt;
	}
	   
	check_particle_bounds(particle);
    }
}

void processInput(void) {
    if (IsKeyPressed(KEY_Q)) CloseWindow();
}
