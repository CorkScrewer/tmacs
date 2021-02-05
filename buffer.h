#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "stw.h"

#define MAX_LINE_LENGTH 512

typedef struct {
    stw_vector *lines;
    bool overwrite_mode;
    bool is_one_line, is_return;

    int cursor_x, cursor_y;

    int char_w, char_h;
    TTF_Font *font;
} buffer_t;

buffer_t *buffer_make(TTF_Font *f);
void buffer_free(buffer_t *buffer);

char *buffer_current_line(buffer_t *buffer);

void buffer_type_chars(buffer_t *buffer, char *s, size_t i);
void buffer_newline(buffer_t *buffer);
void buffer_kill_line(buffer_t *buffer);

void buffer_remove_previous_word(buffer_t *buffer);
void buffer_remove_char(buffer_t *buffer, int x, int y);
void buffer_remove_chars(buffer_t *buffer, size_t start, size_t end);

void buffer_tick(SDL_Event *event, buffer_t *buffer);
void buffer_draw(SDL_Renderer *renderer, buffer_t *buffer);

void buffer_cursor_back_indentation(buffer_t *buffer);
void buffer_cursor_forward_word(buffer_t *buffer);
void buffer_cursor_backward_word(buffer_t *buffer);
void buffer_cursor_limit(buffer_t *buffer);
void buffer_cursor_draw(SDL_Renderer *renderer, buffer_t *buffere);
