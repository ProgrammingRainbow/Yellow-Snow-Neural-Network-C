#ifndef FLAKES_H
#define FLAKES_H

#include "main.h"

struct Flake {
        struct Flake *next;
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_Rect rect;
        double y_pos;
        bool is_white;
};

bool flake_new(struct Flake **flakes, SDL_Renderer *renderer,
               SDL_Texture *image, bool is_white, bool gfx_off);
void flakes_free(struct Flake **flakes);
void flake_reset(struct Flake *f, bool full);
void flakes_reset(struct Flake *f, bool full);
int flake_left(const struct Flake *f);
int flake_right(const struct Flake *f);
int flake_bottom(const struct Flake *f);
double flake_normalized_x(const struct Flake *f, double player_x);
double flake_normalized_y(const struct Flake *f, double player_y);
void flakes_update(struct Flake *f, double dt);
void flakes_draw(const struct Flake *f);

#endif
