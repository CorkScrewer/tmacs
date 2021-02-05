#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    char *str;
    TTF_Font *font;

    int char_w, char_h;
} modeline_t;

modeline_t *modeline_make(TTF_Font *font);
void        modeline_free(modeline_t *mode);
void        modeline_draw(SDL_Renderer *renderer, modeline_t *mode, int ww, int wh);
