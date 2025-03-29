#include "flake.h"

bool flake_new(struct Flake **flakes, SDL_Renderer *renderer,
               SDL_Texture *image, bool is_white, bool gfx_off, bool ai_train) {

    struct Flake *new_flake = calloc(1, sizeof(struct Flake));
    if (!new_flake) {
        fprintf(stderr, "Error in calloc of new flake!\n");
        return false;
    }

    new_flake->is_white = is_white;
    new_flake->ai_train = ai_train;

    if (!gfx_off) {
        new_flake->renderer = renderer;
        new_flake->image = image;

        if (SDL_QueryTexture(image, NULL, NULL, &new_flake->rect.w,
                             &new_flake->rect.h)) {
            fprintf(stderr, "Error while querying texture: %s\n",
                    SDL_GetError());
            return false;
        }
    } else {
        new_flake->rect.h = 32;
        new_flake->rect.w = 32;
    }

    flake_reset(new_flake, true);

    if (*flakes) {
        new_flake->next = *flakes;
    } else {
        new_flake->next = NULL;
    }
    *flakes = new_flake;

    return true;
}

void flakes_free(struct Flake **flakes) {
    struct Flake *f = *flakes;
    while (f) {
        f->image = NULL;
        f->renderer = NULL;
        struct Flake *next = f->next;
        free(f);
        f = next;
    }
    *flakes = NULL;
}

void flake_reset(struct Flake *f, bool full) {
    int height = full ? WINDOW_HEIGHT * 2 : WINDOW_HEIGHT;
    f->rect.x = (rand() % (WINDOW_WIDTH + f->rect.w)) - f->rect.w;
    f->rect.y = -((rand() % height) + f->rect.h);
    f->y_pos = f->rect.y;
}

void flakes_reset(struct Flake *f, bool full) {
    while (f) {
        flake_reset(f, full);
        f = f->next;
    }
}

int flake_left(const struct Flake *f) { return f->rect.x; }

int flake_right(const struct Flake *f) { return f->rect.x + f->rect.w; }

int flake_bottom(const struct Flake *f) { return f->rect.y + f->rect.h; }

double flake_normalized_x(const struct Flake *f, double player_x) {
    return ((player_x - f->rect.x - (f->rect.w / 2.0)) /
            (WINDOW_WIDTH + f->rect.w));
}

double flake_normalized_y(const struct Flake *f, double player_y) {
    return ((player_y - f->rect.y - (f->rect.h / 2.0)) / (WINDOW_HEIGHT * 2));
}

void flakes_update(struct Flake *f, double dt) {
    while (f) {
        if (f->ai_train) {
            f->rect.y += FLAKE_AI_SPEED;
            if (f->rect.y > GROUND) {
                flake_reset(f, false);
            }
        } else {
            f->y_pos += FLAKE_SPEED * dt;
            if (f->y_pos > GROUND) {
                flake_reset(f, false);
            } else {
                f->rect.y = (int)f->y_pos;
            }
        }
        f = f->next;
    }
}

void flakes_draw(const struct Flake *f) {
    while (f) {
        SDL_RenderCopy(f->renderer, f->image, NULL, &f->rect);
        f = f->next;
    }
}
