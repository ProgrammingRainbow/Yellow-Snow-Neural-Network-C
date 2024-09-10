#include "player.h"

bool player_new(struct Player **player, SDL_Renderer *renderer,
                SDL_Texture *image, bool gfx_on) {
    *player = calloc(1, sizeof(struct Player));

    if (!*player) {
        fprintf(stderr, "Error in calloc of player!\n");
        return false;
    }
    struct Player *p = *player;

    p->gfx_on = gfx_on;

    if (p->gfx_on) {
        p->renderer = renderer;
        p->image = image;

        if (SDL_QueryTexture(p->image, NULL, NULL, &p->rect.w, &p->rect.h)) {
            fprintf(stderr, "Error while querying texture: %s\n",
                    SDL_GetError());
            return false;
        }

        p->keystate = SDL_GetKeyboardState(NULL);
    } else {
        p->rect.h = 158;
        p->rect.w = 145;
    }

    p->flip = SDL_FLIP_HORIZONTAL;
    p->x_pos = (double)(WINDOW_WIDTH - p->rect.w) / 2;
    p->rect.x = (int)p->x_pos;
    p->rect.y = PLAYER_X;

    return true;
}

void player_free(struct Player **player) {
    if (*player) {
        (*player)->image = NULL;
        (*player)->renderer = NULL;
        (*player)->keystate = NULL;
        free(*player);
        *player = NULL;
    }
}

void player_reset(struct Player *p) {
    p->x_pos = (double)(WINDOW_WIDTH - p->rect.w) / 2;
    p->rect.x = (int)p->x_pos;
}

int player_left(struct Player *p) { return p->rect.x + PLAYER_LEFT_OFFSET; }

int player_right(struct Player *p) {
    return p->rect.x + p->rect.w - PLAYER_RIGHT_OFFSET;
}

int player_top(struct Player *p) { return p->rect.y + PLAYER_TOP_OFFSET; }

void player_update(struct Player *p, double dt, bool left, bool right) {

    bool move_left = false;
    bool move_right = false;

    if (p->gfx_on) {
        move_left = p->keystate[SDL_SCANCODE_LEFT] || left;
        move_right = p->keystate[SDL_SCANCODE_RIGHT] || right;
    } else {
        move_left = left;
        move_right = right;
    }

    if (move_left) {
        p->x_pos -= PLAYER_SPEED * dt;
        if (p->x_pos + PLAYER_LEFT_OFFSET < 0) {
            p->x_pos = -PLAYER_LEFT_OFFSET;
        }
        p->rect.x = (int)(p->x_pos + 0.5);
        p->flip = SDL_FLIP_HORIZONTAL;
    }
    if (move_right) {
        p->x_pos += PLAYER_SPEED * dt;
        if (p->x_pos + p->rect.w - PLAYER_RIGHT_OFFSET > WINDOW_WIDTH) {
            p->x_pos = WINDOW_WIDTH + PLAYER_RIGHT_OFFSET - p->rect.w;
        }
        p->rect.x = (int)(p->x_pos + 0.5);
        p->flip = SDL_FLIP_NONE;
    }
}

void player_draw(struct Player *p) {
    SDL_RenderCopyEx(p->renderer, p->image, NULL, &p->rect, 0, NULL, p->flip);
}
