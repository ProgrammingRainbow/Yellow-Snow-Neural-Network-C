#ifndef SCORE_H
#define SCORE_H

#include "main.h"

struct Score {
        SDL_Renderer *renderer;
        TTF_Font *font;
        SDL_Surface *surface;
        SDL_Texture *image;
        SDL_Rect rect;
        int score;
        bool gfx_off;
        int white_inc;
        int yellow_inc;
};

bool score_new(struct Score **score, SDL_Renderer *renderer,
               const struct AIConfig *config);
void score_free(struct Score **score);
bool score_reset(struct Score *s);
bool score_increment(struct Score *s);
bool score_decrement(struct Score *s);
void score_draw(const struct Score *s);

#endif
