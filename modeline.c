#include "modeline.h"

#include <string.h>
#include <stdlib.h>

modeline_t *modeline_make(TTF_Font *font) {
    modeline_t *m = malloc(sizeof(modeline_t));
    m->str = calloc(512, 1);
    m->font = font;

    TTF_SizeText(m->font, ".", &m->char_w, &m->char_h);

    strcat(m->str, "*buffer*");

    return m;
}

void modeline_free(modeline_t *mode) {
    free(mode->str);
    free(mode);
}

void modeline_draw(SDL_Renderer *renderer, modeline_t *mode, int ww, int wh) {
    const SDL_Color textcol = { 0, 0, 0, 0 };

    const SDL_Rect bar = {
        0,
        wh - mode->char_h*2,
        ww,
        mode->char_h
    };
    SDL_SetRenderDrawColor(renderer, 215, 215, 215, 215);
    SDL_RenderFillRect(renderer, &bar);

    if (mode->str[0] == 0) return;
    
    SDL_Surface *surf = TTF_RenderText_Solid(mode->font, mode->str, textcol);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);

    const SDL_Rect dst = {
        mode->char_w,
        wh - mode->char_h*2,
        surf->w,
        surf->h
    };

    SDL_RenderCopy(renderer, texture, NULL, &dst);
}
