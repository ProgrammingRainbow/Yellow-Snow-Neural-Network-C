#include "score.h"

bool score_update(struct Score *s);

bool score_new(struct Score **score, SDL_Renderer *renderer,
               const struct AIConfig *config) {

    *score = calloc(1, sizeof(struct Score));
    if (!*score) {
        fprintf(stderr, "Error in calloc of score!\n");
        return false;
    }

    struct Score *s = *score;

    s->gfx_off = config->gfx_off;
    s->white_inc = config->white_inc;
    s->yellow_inc = config->yellow_inc;
    s->rect.x = SCORE_X;
    s->rect.y = SCORE_Y;

    if (!s->gfx_off) {
        s->renderer = renderer;

        s->font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
        if (!s->font) {
            fprintf(stderr, "Error creating font: %s\n", TTF_GetError());
            return false;
        }
    }

    if (!score_reset(s)) {
        return false;
    }

    return true;
}

void score_free(struct Score **score) {
    if (*score) {
        struct Score *s = *score;

        if (s->image) {
            SDL_DestroyTexture(s->image);
            s->image = NULL;
        }
        if (s->surface) {
            SDL_FreeSurface(s->surface);
            s->surface = NULL;
        }
        if (s->font) {
            TTF_CloseFont(s->font);
            s->font = NULL;
        }

        s->renderer = NULL;

        free(s);
        s = NULL;
        *score = NULL;
    }
}

bool score_reset(struct Score *s) {
    s->score = 0;
    if (!score_update(s)) {
        return false;
    }

    return true;
}

bool score_update(struct Score *s) {
    if (!s->gfx_off) {
        if (s->surface) {
            SDL_FreeSurface(s->surface);
            s->surface = NULL;
        }
        if (s->image) {
            SDL_DestroyTexture(s->image);
            s->image = NULL;
        }

        int length = snprintf(NULL, 0, "Score: %d", s->score) + 1;
        char score_str[length];
        snprintf(score_str, (size_t)length, "Score: %d", s->score);

        s->surface = TTF_RenderText_Blended(s->font, score_str, FONT_COLOR);
        if (!s->surface) {
            fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
            return false;
        }

        s->rect.w = s->surface->w;
        s->rect.h = s->surface->h;

        s->image = SDL_CreateTextureFromSurface(s->renderer, s->surface);
        if (!s->image) {
            fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
            return false;
        }
    }

    return true;
}

bool score_increment(struct Score *s) {
    s->score += s->white_inc;
    if (!score_update(s)) {
        return false;
    }

    return true;
}

bool score_decrement(struct Score *s) {
    s->score += s->yellow_inc;
    if (!score_update(s)) {
        return false;
    }

    return true;
}

void score_draw(const struct Score *s) {
    SDL_RenderCopy(s->renderer, s->image, NULL, &s->rect);
}
