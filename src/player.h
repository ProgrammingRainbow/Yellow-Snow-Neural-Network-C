#ifndef PLAYER_H
#define PLAYER_H

#include "main.h"

#define PLAYER_X 377
#define PLAYER_TOP_OFFSET 10
#define PLAYER_LEFT_OFFSET 47
#define PLAYER_RIGHT_OFFSET 43
#define PLAYER_SPEED 300

struct Player {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_Rect rect;
        SDL_RendererFlip flip;
        const Uint8 *keystate;
        double x_pos;
        bool gfx_on;
};

bool player_new(struct Player **player, SDL_Renderer *renderer,
                SDL_Texture *image, bool gfx_on);
void player_free(struct Player **player);
void player_reset(struct Player *p);
int player_left(struct Player *p);
int player_right(struct Player *p);
int player_top(struct Player *p);
void player_update(struct Player *p, double dt, bool left, bool right);
void player_draw(struct Player *p);

#endif
