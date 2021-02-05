#pragma once

#include "stw.h"
#include "buffer.h"

typedef struct {
    char *start;
    buffer_t *buffer;
    bool is_focused;
} infobar_t;

infobar_t *infobar_make(TTF_Font *font);
void infobar_tick(SDL_Event *event, infobar_t *i);
void infobar_draw(infobar_t *i, SDL_Renderer *renderer);
