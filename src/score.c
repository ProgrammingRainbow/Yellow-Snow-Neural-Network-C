#include "score.h"

bool score_update(struct Score *s);

bool score_new(struct Score **score, SDL_Renderer *renderer, bool gfx_on) {

    *score = calloc(1, sizeof(struct Score));
    if (!*score) {
        fprintf(stderr, "Error in calloc of score!\n");
        return false;
    }

    struct Score *s = *score;

    s->gfx_on = gfx_on;
    s->rect.x = SCORE_X;
    s->rect.y = SCORE_Y;

    if (s->gfx_on) {
        s->renderer = renderer;
        s->color = (SDL_Color){255, 255, 255, 255};

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

        if (s->font) {
            TTF_CloseFont(s->font);
            s->font = NULL;
        }
        if (s->image) {
            SDL_DestroyTexture(s->image);
            s->image = NULL;
        }
        s->renderer = NULL;

        free(s);
        s = NULL;
        *score = NULL;
    }
}

bool score_reset(struct Score *s) {
    s->score = 0;
    if (s->gfx_on) {
        if (!score_update(s)) {
            return false;
        }
    }

    return true;
}

bool score_update(struct Score *s) {
    if (s->image) {
        SDL_DestroyTexture(s->image);
        s->image = NULL;
    }

    int length = snprintf(NULL, 0, "Score: %d", s->score) + 1;
    char score_str[length];
    snprintf(score_str, (size_t)length, "Score: %d", s->score);

    SDL_Surface *font_surf =
        TTF_RenderText_Blended(s->font, score_str, s->color);
    if (!font_surf) {
        fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
        return false;
    }

    s->rect.w = font_surf->w;
    s->rect.h = font_surf->h;

    s->image = SDL_CreateTextureFromSurface(s->renderer, font_surf);
    SDL_FreeSurface(font_surf);
    if (!s->image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

bool score_increment(struct Score *s) {
    s->score++;
    if (s->gfx_on) {
        if (!score_update(s)) {
            return false;
        }
    }

    return true;
}

void score_draw(struct Score *s) {
    SDL_RenderCopy(s->renderer, s->image, NULL, &s->rect);
}
