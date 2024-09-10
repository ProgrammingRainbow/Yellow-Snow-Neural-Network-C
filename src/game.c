#include "game.h"
#include "initialize.h"
#include "load_media.h"
#include "neural_network.h"

bool game_reset(struct Game *g);
bool handle_collision(struct Game *g, struct Flake *f);
bool check_collision(struct Game *g);
bool game_update(struct Game *g);
void game_draw(struct Game *g);
void game_ai_update(struct Game *g);

bool game_new(struct Game *g, struct AIConfig ai_config) {

    g->ai_on = ai_config.ai_on;
    g->ai_train = ai_config.ai_train;
    g->gfx_on = ai_config.gfx_on;
    g->game_variant_count = ai_config.game_variant_count;
    g->trial_count = ai_config.trial_count;

    g->networks =
        calloc((size_t)g->game_variant_count, sizeof(struct NeuralNetwork));
    if (!g->networks) {
        fprintf(stderr, "Error in calloc of NeuralNetwork array!\n");
        return false;
    }

    srand((Uint32)time(NULL));

    if (g->gfx_on) {
        if (!sdl_initilize(g)) {
            return false;
        }

        if (!game_load_media(g)) {
            return false;
        }
    }

    if (!player_new(&g->player, g->renderer, g->player_image, g->gfx_on)) {
        return false;
    }

    for (unsigned int i = 0; i < 10; i++) {
        if (!flake_new(&g->flakes, g->renderer, g->white_image, true,
                       g->gfx_on)) {
            return false;
        }
    }

    for (unsigned int i = 0; i < 5; i++) {
        if (!flake_new(&g->flakes, g->renderer, g->yellow_image, false,
                       g->gfx_on)) {
            return false;
        }
    }

    if (!score_new(&g->score, g->renderer, g->gfx_on)) {
        return false;
    }

    if (!fps_new(&g->fps)) {
        return false;
    }

    g->playing = true;
    g->running = true;

    return true;
}

void game_free(struct Game *g) {

    player_free(&g->player);
    flakes_free(&g->flakes);
    score_free(&g->score);
    fps_free(&g->fps);

    if (g->gfx_on) {
        if (g->hit_sound) {
            Mix_FreeChunk(g->hit_sound);
            g->hit_sound = NULL;
        }
        if (g->collect_sound) {
            Mix_FreeChunk(g->collect_sound);
            g->collect_sound = NULL;
        }
        if (g->winter_music) {
            Mix_FreeMusic(g->winter_music);
            g->winter_music = NULL;
        }

        if (g->white_image) {
            SDL_DestroyTexture(g->white_image);
            g->white_image = NULL;
        }
        if (g->yellow_image) {
            SDL_DestroyTexture(g->yellow_image);
            g->yellow_image = NULL;
        }
        if (g->player_image) {
            SDL_DestroyTexture(g->player_image);
            g->player_image = NULL;
        }
        if (g->background_image) {
            SDL_DestroyTexture(g->background_image);
            g->background_image = NULL;
        }

        if (g->renderer) {
            SDL_DestroyRenderer(g->renderer);
            g->renderer = NULL;
        }
        if (g->window) {
            SDL_DestroyWindow(g->window);
            g->window = NULL;
        }

        Mix_CloseAudio();

        TTF_Quit();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    if (g->networks) {
        free(g->networks);
    }
}

bool game_reset(struct Game *g) {
    flakes_reset(g->flakes, true);

    if (!score_reset(g->score)) {
        return false;
    }

    if (g->gfx_on) {
        if (Mix_PlayMusic(g->winter_music, -1)) {
            fprintf(stderr, "Error while playing music: %s\n", Mix_GetError());
            return false;
        }
    }

    if (g->ai_on) {
        player_reset(g->player);
    }

    g->playing = true;

    return true;
}

bool handle_collision(struct Game *g, struct Flake *f) {
    if (f->is_white) {
        if (g->gfx_on) {
            Mix_PlayChannel(-1, g->collect_sound, 0);
        }
        if (!score_increment(g->score)) {
            return false;
        }
        flake_reset(f, false);
    } else {
        if (g->gfx_on) {
            Mix_HaltMusic();
            Mix_PlayChannel(-1, g->hit_sound, 0);
        }

        if (g->ai_train) {
            game_ai_update(g);
        } else {
            g->playing = false;
        }
    }

    return true;
}

void game_ai_update(struct Game *g) {
    g->networks[g->current_variant].fitness += g->score->score;
    g->current_trial++;
    if (g->current_trial < g->trial_count) {
        game_reset(g);
    } else {
        g->networks[g->current_variant].fitness /= g->trial_count;
        g->current_trial = 0;
        g->current_variant++;
        if (g->current_variant < g->game_variant_count) {
            game_reset(g);
        } else {
            g->current_variant = 0;
            g->running = false;
        }
    }
}

bool check_collision(struct Game *g) {
    struct Flake *f = g->flakes;
    while (f) {
        if (flake_bottom(f) > player_top(g->player)) {
            if (flake_right(f) > player_left(g->player)) {
                if (flake_left(f) < player_right(g->player)) {
                    if (!handle_collision(g, f)) {
                        return false;
                    }
                }
            }
        }
        f = f->next;
    }
    return true;
}

bool game_update(struct Game *g) {
    flakes_update(g->flakes, g->delta_time);

    if (g->ai_on) {
        g->networks[g->current_variant].inputs[0] =
            (double)player_left(g->player) / WINDOW_WIDTH;

        int input = 1;
        struct Flake *flake = g->flakes;
        while (flake) {
            g->networks[g->current_variant].inputs[input] =
                (double)(flake->rect.x) / WINDOW_WIDTH;
            g->networks[g->current_variant].inputs[input + 1] =
                (double)(flake->rect.y) / WINDOW_HEIGHT;
            input += 2;
            flake = flake->next;
        }

        network_update(&g->networks[g->current_variant]);

        player_update(g->player, g->delta_time,
                      g->networks[g->current_variant].final_output[0],
                      g->networks[g->current_variant].final_output[1]);
    } else {
        player_update(g->player, g->delta_time, false, false);
    }

    if (!check_collision(g)) {
        return false;
    }

    return true;
}

void game_draw(struct Game *g) {
    SDL_RenderClear(g->renderer);

    SDL_RenderCopy(g->renderer, g->background_image, NULL, &g->background_rect);

    player_draw(g->player);

    flakes_draw(g->flakes);

    score_draw(g->score);

    SDL_RenderPresent(g->renderer);
}

void *game_run_multi(void *arg) {
    struct Game *g = (struct Game *)arg;
    g->running = true;
    g->delta_time = 0.0167;
    while (g->running) {
        game_update(g);
    }

    return NULL;
}

bool game_run(struct Game *g) {
    if (g->gfx_on) {
        if (Mix_PlayMusic(g->winter_music, -1)) {
            fprintf(stderr, "Error while playing music: %s\n", Mix_GetError());
            return false;
        }
    }

    g->running = true;
    while (g->running) {
        if (g->gfx_on) {
            while (SDL_PollEvent(&g->event)) {
                switch (g->event.type) {
                case SDL_QUIT:
                    return true;
                    break;
                case SDL_KEYDOWN:
                    switch (g->event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        return true;
                        break;
                    case SDL_SCANCODE_SPACE:
                        if (!g->playing) {
                            if (!game_reset(g)) {
                                return false;
                            }
                        }
                        break;
                    case SDL_SCANCODE_F:
                        fps_toggle_display(g->fps);
                        break;
                    default:
                        break;
                    }
                default:
                    break;
                }
            }
        }

        if (g->playing) {
            game_update(g);
        }

        if (g->gfx_on) {
            game_draw(g);
        }

        if (g->ai_train) {
            g->delta_time = 0.0167;
        } else {
            g->delta_time = fps_update(g->fps);
        }
    }

    return true;
}
