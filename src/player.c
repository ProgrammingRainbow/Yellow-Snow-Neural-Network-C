#include "player.h"

bool player_new(struct Player **player, SDL_Renderer *renderer,
                SDL_Texture *image, bool gfx_off, bool ai_train) {
    *player = calloc(1, sizeof(struct Player));
    if (!*player) {
        fprintf(stderr, "Error in calloc of player!\n");
        return false;
    }
    struct Player *p = *player;

    p->ai_train = ai_train;

    if (!gfx_off) {
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
    p->rect.y = PLAYER_Y;
    p->normalized_y = (PLAYER_Y + (p->rect.h / 2.0) + (WINDOW_HEIGHT / 2.0)) /
                      (1.5 * WINDOW_HEIGHT);

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

int player_left(const struct Player *p) {
    return p->rect.x + PLAYER_LEFT_OFFSET;
}

int player_right(const struct Player *p) {
    return p->rect.x + p->rect.w - PLAYER_RIGHT_OFFSET;
}

int player_top(const struct Player *p) { return p->rect.y + PLAYER_TOP_OFFSET; }

double player_center_x(const struct Player *p) {
    return p->rect.x + (p->rect.w / 2.0);
}

double player_center_y(const struct Player *p) {
    return p->rect.y + (p->rect.h / 2.0);
}

double player_normalized_x(const struct Player *p) {
    return ((p->rect.x + (p->rect.w / 2.0) + 16) / (WINDOW_WIDTH + 32)) * 2 - 1;
}

double player_normalized_y(const struct Player *p) { return p->normalized_y; }

void player_update(struct Player *p, double dt, bool left, bool right) {
    if (p->ai_train) {
        if (left) {
            p->rect.x -= PLAYER_AI_SPEED;
            if (p->rect.x + (p->rect.w / 2) < 0) {
                p->rect.x = WINDOW_WIDTH - (p->rect.w / 2);
            }
        }
        if (right) {
            p->rect.x += PLAYER_AI_SPEED;
            if (p->rect.x > WINDOW_WIDTH - (p->rect.w / 2)) {
                p->rect.x = -(p->rect.w / 2);
            }
        }
    } else {
        if (p->keystate[SDL_SCANCODE_LEFT] || p->keystate[SDL_SCANCODE_A] ||
            left) {
            p->x_pos -= PLAYER_SPEED * dt;
            if (p->x_pos + (p->rect.w / 2.0) < 0) {
                p->x_pos = WINDOW_WIDTH - (p->rect.w / 2.0);
            }
            p->flip = SDL_FLIP_HORIZONTAL;
        }
        if (p->keystate[SDL_SCANCODE_RIGHT] || p->keystate[SDL_SCANCODE_D] ||
            right) {
            p->x_pos += PLAYER_SPEED * dt;
            if (p->x_pos > WINDOW_WIDTH - (p->rect.w / 2.0)) {
                p->x_pos = -(p->rect.w / 2.0);
            }
            p->flip = SDL_FLIP_NONE;
        }
        p->rect.x = (int)(p->x_pos);
    }
}

void player_draw(const struct Player *p) {
    SDL_RenderCopyEx(p->renderer, p->image, NULL, &p->rect, 0, NULL, p->flip);
}
