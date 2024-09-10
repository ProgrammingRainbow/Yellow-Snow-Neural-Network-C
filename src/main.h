#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_TITLE "Don't Eat the Yellow Snow!"
#define WINDOW_ICON "images/yellow.png"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define SDL_FLAGS SDL_INIT_EVERYTHING
#define IMG_FLAGS IMG_INIT_PNG
#define MIX_FLAGS MIX_INIT_OGG
#define CHUNK_SIZE 1024

#define TARGET_FPS 60
#define SHOW_FPS false

#define SAVED_NETWORKS "neural-networks"
#define MUTATION_RATE 0.5
#define MUTATION_PROB 0.05

#define INPUTS 31
#define FIRST_HIDDEN 15
#define SECOND_HIDDEN 15
#define OUTPUTS 2

#define THREADS 1
#define TOTAL_TRIALS 1
#define TOTAL_VARIANTS 200
#define TOTAL_GENS 1
#define TOP_VARIANTS 32

struct AIConfig {
        bool ai_on;
        bool ai_train;
        bool gfx_on;
        int thread_count;
        int trial_count;
        int variant_count;
        int generation_count;
        int game_variant_count;
        int top_variant_count;
};

#endif
