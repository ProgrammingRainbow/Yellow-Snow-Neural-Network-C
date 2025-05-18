#ifndef PLAYER_H
#define PLAYER_H

#include "main.h"

struct Player {
        SDL_Renderer *renderer;
        SDL_Texture *image;
        SDL_Rect rect;
        SDL_RendererFlip flip;
        const Uint8 *keystate;
        double x_pos;
        double normalized_y;
        bool ai_train;
};

bool player_new(struct Player **player, SDL_Renderer *renderer,
                SDL_Texture *image, bool gfx_on, bool ai_train);
void player_free(struct Player **player);
void player_reset(struct Player *p);
int player_left(const struct Player *p);
int player_right(const struct Player *p);
int player_top(const struct Player *p);
double player_center_x(const struct Player *p);
double player_center_y(const struct Player *p);
double player_normalized_x(const struct Player *p);
double player_normalized_y(const struct Player *p);
void player_update(struct Player *p, double dt, bool left, bool right);
void player_draw(const struct Player *p);

#endif
