#ifndef GAME_H
#define GAME_H

#include "flake.h"
#include "fps.h"
#include "main.h"
#include "player.h"
#include "score.h"

struct Game {
        SDL_Event event;
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *background_image;
        SDL_Rect background_rect;
        SDL_Texture *player_image;
        SDL_Texture *white_image;
        SDL_Texture *yellow_image;
        Mix_Music *winter_music;
        Mix_Chunk *collect_sound;
        Mix_Chunk *hit_sound;
        struct Player *player;
        struct Flake *flakes;
        struct Score *score;
        struct Fps *fps;
        struct NeuralNetwork *networks;
        bool playing;
        bool running;
        double delta_time;
        bool ai_on;
        bool ai_train;
        bool gfx_on;
        int game_variant_count;
        int trial_count;
        int current_variant;
        int current_trial;
};

bool game_new(struct Game *game, struct AIConfig ai_config);
void game_free(struct Game *game);
bool game_run(struct Game *g);
void *game_run_multi(void *arg);

#endif
