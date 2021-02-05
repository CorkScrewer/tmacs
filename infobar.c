#include "infobar.h"

infobar_t *infobar_make(TTF_Font *font) {
    infobar_t *i = malloc(sizeof(infobar_t));
    i->buffer = buffer_make(font);
    i->buffer->is_one_line = true;
    i->is_focused = false;
    i->start = NULL;

    return i;
}

void infobar_push(infobar_t *i, char *msg) {
    i->is_focused = true;
    strcpy(i->start, msg);
}

void infobar_tick(SDL_Event *event, infobar_t *i) {
    if (i->is_focused) {
        buffer_tick(event, i->buffer);

        if (i->buffer->is_return) {
            /* TODO: Switch through all possible infobar stuff, and handle them here. */
            printf("Enter: %s\n", (char*)i->buffer->lines->array[0]);
            i->is_focused = false;
        }
    }
}

void infobar_draw(infobar_t *i, SDL_Renderer *renderer) {
    if (i->is_focused) {
        buffer_draw(renderer, i->buffer);
    }
}
