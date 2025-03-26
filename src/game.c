#include "game.h"
#include "initialize.h"
#include "load_media.h"

bool game_reset(struct Game *g);
bool handle_collision(struct Game *g, struct Flake *f);
bool check_collision(struct Game *g);
bool game_update(struct Game *g);
void game_draw(const struct Game *g);
void game_ai_update(struct Game *g);

bool game_new(struct Game *g, const struct AIConfig *config) {

    g->ai_on = config->ai_on;
    g->ai_train = config->ai_train;
    g->gfx_off = config->gfx_off;
    g->fit_style = config->fit_style;
    g->max_ticks = config->max_ticks;
    g->game_variant_count = config->game_variant_count;
    g->trial_count = config->trial_count;

    g->networks =
        calloc((size_t)g->game_variant_count, sizeof(struct NeuralNetwork));
    if (!g->networks) {
        fprintf(stderr, "Error in calloc of NeuralNetwork array!\n");
        return false;
    }

    srand((Uint32)time(NULL));

    if (!g->gfx_off) {
        if (!sdl_initilize(g)) {
            return false;
        }

        if (!game_load_media(g)) {
            return false;
        }
    }

    if (!player_new(&g->player, g->renderer, g->player_image, g->gfx_off)) {
        return false;
    }

    for (unsigned int i = 0; i < WHITE_FLAKES; i++) {
        if (!flake_new(&g->flakes, g->renderer, g->white_image, true,
                       g->gfx_off)) {
            return false;
        }
    }

    for (unsigned int i = 0; i < YELLOW_FLAKES; i++) {
        if (!flake_new(&g->flakes, g->renderer, g->yellow_image, false,
                       g->gfx_off)) {
            return false;
        }
    }

    if (!score_new(&g->score, g->renderer, config)) {
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

    if (!g->gfx_off) {
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

    if (!g->gfx_off) {
        if (Mix_PlayMusic(g->winter_music, -1)) {
            fprintf(stderr, "Error while playing music: %s\n", Mix_GetError());
            return false;
        }
    }

    if (g->ai_on) {
        player_reset(g->player);
    }

    if (g->ai_train) {
        g->game_ticks = 0;
    }

    g->playing = true;

    return true;
}

bool handle_collision(struct Game *g, struct Flake *f) {
    if (f->is_white) {
        if (!g->gfx_off) {
            Mix_PlayChannel(-1, g->collect_sound, 0);
        }
        if (!score_increment(g->score)) {
            return false;
        }
        flake_reset(f, false);
    } else {
        if (!g->gfx_off) {
            Mix_HaltMusic();
            Mix_PlayChannel(-1, g->hit_sound, 0);
        }

        if (g->ai_train) {
            if (g->fit_style == FIT_TIMED) {
                if (!score_decrement(g->score)) {
                    return false;
                }
                flake_reset(f, false);
            } else {
                game_ai_update(g);
            }
        } else {
            g->playing = false;
        }
    }

    return true;
}

void game_ai_update(struct Game *g) {
    if (g->fit_style == FIT_TICKS) {
        g->networks[g->current_variant].fitness += g->game_ticks;
    } else {
        g->networks[g->current_variant].fitness += g->score->score;
    }

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
    /*printf("fitness: %d\n", g->networks[g->current_variant].fitness);*/
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

    // printf("player x: %f\n", player_center_x(g->player));
    // printf("player y: %f\n", player_center_y(g->player));

    // printf("flake x: %f\n",
    //        flake_normalized_x(g->flakes, player_center_x(g->player)));
    // printf("flake y: %f\n",
    //        flake_normalized_y(g->flakes, player_center_y(g->player)));

    if (g->ai_on) {
        g->networks[g->current_variant].inputs[0] =
            player_normalized_x(g->player);

        int input = 1;
        struct Flake *flake = g->flakes;
        while (flake) {
            g->networks[g->current_variant].inputs[input] =
                flake_normalized_x(flake, player_center_x(g->player));
            g->networks[g->current_variant].inputs[input + 1] =
                flake_normalized_y(flake, player_center_y(g->player));
            input += 2;
            flake = flake->next;
        }

        network_update(&g->networks[g->current_variant]);

        player_update(g->player, g->delta_time,
                      g->networks[g->current_variant].output[0],
                      g->networks[g->current_variant].output[1]);
    } else {
        player_update(g->player, g->delta_time, false, false);
    }

    if (!check_collision(g)) {
        return false;
    }

    if (g->ai_train) {
        g->game_ticks++;
        if (g->fit_style == FIT_TIMED) {
            if (g->game_ticks > g->max_ticks) {
                game_ai_update(g);
            }
        }
    }

    return true;
}

void game_draw(const struct Game *g) {
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
    if (!g->gfx_off) {
        if (Mix_PlayMusic(g->winter_music, -1)) {
            fprintf(stderr, "Error while playing music: %s\n", Mix_GetError());
            return false;
        }
    }

    g->running = true;
    while (g->running) {
        if (!g->gfx_off) {
            while (SDL_PollEvent(&g->event)) {
                switch (g->event.type) {
                case SDL_QUIT:
                    g->running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (g->event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        g->running = false;
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
                    break;
                default:
                    break;
                }
            }
        }

        if (g->playing) {
            game_update(g);
        }

        if (!g->gfx_off) {
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
