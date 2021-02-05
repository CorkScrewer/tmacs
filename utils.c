#include "utils.h"

#include <SDL2/SDL.h>

#include <string.h>

void string_insert(char *subject, const char *insert, size_t pos) {
    size_t insert_len = strlen(insert);
    char *buf = calloc(strlen(subject) + insert_len + 2, 1);

    strncpy(buf, subject, pos);
    size_t len = strlen(buf);
    strcpy(buf + len, insert);
    len += insert_len;
    strcpy(buf + len, subject + pos);

    strcpy(subject, buf);

    free(buf);
}

bool is_control_held(const u8 *keys) {
    return (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]);
}

bool is_meta_held(const u8 *keys) {
    return (keys[SDL_SCANCODE_LALT] || keys[SDL_SCANCODE_RALT]);
}
