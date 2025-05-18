#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
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

#define FONT_FILE "fonts/freesansbold.ttf"
#define FONT_SIZE 24
#define FONT_COLOR (SDL_Color){255, 255, 255, 255}
#define SCORE_X 10
#define SCORE_Y 10

#define PLAYER_Y 377
#define PLAYER_TOP_OFFSET 10
#define PLAYER_LEFT_OFFSET 45
#define PLAYER_RIGHT_OFFSET 45
#define PLAYER_SPEED 300
#define PLAYER_AI_SPEED 5

#define FLAKE_SPEED 300
#define FLAKE_AI_SPEED 5
#define WHITE_FLAKES 10
#define YELLOW_FLAKES 5
#define GROUND 514

#define WHITE_INC 1
#define YELLOW_INC -2

#define INFO_DELAY 3000
#define FILE_NAME "neural-networks"

#define INPUTS 30
#define OUTPUTS 2

#define LAYER_SIZE 64
#define LAYERS 10

#define MUT_RATE 5
#define MAX_RATE 15

#define GAU_DEV 10
#define INT_DEV 5
#define MAX_DEV 300

#define STD_DEVIATION 1

#define THREADS 1
#define MAX_THREADS 24

#define TICKS 1800
#define MAX_TICKS 7200

#define TRIALS 1
#define MAX_TRIALS 100

#define GENS 20
#define MAX_GENS 1000000

#define VARIANTS 240
#define MAX_VARIANTS 1024

#define TOP_VARIANTS 24
#define MAX_TOP_VARIANTS 512

enum Fitness { FIT_SCORE, FIT_TICKS, FIT_TIMED };

struct AIConfig {
        int layer_sizes[LAYERS];
        int layer_count;
        const char *filename;
        int white_inc;
        int yellow_inc;
        bool ai_on;
        bool ai_train;
        bool gfx_off;
        enum Fitness fit_style;
        double mut_rate;
        double int_dev;
        double gau_dev;
        int thread_count;
        int max_ticks;
        int trial_count;
        int variant_count;
        int generation_count;
        int game_variant_count;
        int top_variant_count;
};

#endif
