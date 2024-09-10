#ifndef SCORE_H
#define SCORE_H

#include "main.h"

#define FONT_FILE "fonts/freesansbold.ttf"
#define FONT_SIZE 24
#define SCORE_X 10
#define SCORE_Y 10

struct Score {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        TTF_Font *font;
        SDL_Color color;
        SDL_Rect rect;
        int score;
        bool gfx_on;
};

bool score_new(struct Score **score, SDL_Renderer *renderer, bool gfx_on);
void score_free(struct Score **score);
bool score_reset(struct Score *s);
bool score_increment(struct Score *s);
void score_draw(struct Score *s);

#endif
