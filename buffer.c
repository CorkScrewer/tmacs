#include "buffer.h"
#include "utils.h"
#include "infobar.h"

#include <ctype.h>

buffer_t *buffer_make(TTF_Font *f) {
    buffer_t *buffer = malloc(sizeof(buffer_t));
    if (!buffer) return NULL;

    buffer->cursor_x = 0;
    buffer->cursor_y = 0;
    buffer->is_one_line = false;
    
    buffer->lines = stw_vector_make();
    buffer->overwrite_mode = false;
    
    stw_vector_push(buffer->lines, calloc(MAX_LINE_LENGTH, 1));

    buffer->font = f;
    TTF_SizeText(f, ".", &buffer->char_w, &buffer->char_h);

    return buffer;
}

void buffer_free(buffer_t *buffer) {
    stw_vector_free(buffer->lines, true);
    free(buffer);
}

void buffer_type_chars(buffer_t *buffer, char *s, size_t i) {
    size_t len = strlen(s);
    char *cl = buffer_current_line(buffer);
    
    if (len == MAX_LINE_LENGTH) return;

    if (buffer->overwrite_mode) {
        for (size_t it = 0; it < len; ++it) {
            cl[i + it] = s[it];
        }
    } else {
        string_insert(cl, s, i);
    }
    buffer->cursor_x += len;
}

void buffer_remove_char(buffer_t *buffer, int x, int y) {
    if (x < 0) return;
    if (y < 0) return;
    
    char *s = (char *) buffer->lines->array[y];
    size_t len = strlen(s);

    for (size_t i = x; i < len; ++i) {
        s[i] = s[i+1];
    }
}

void buffer_remove_chars(buffer_t *buffer, size_t start, size_t end) {
    char *s = buffer_current_line(buffer);

    for (size_t i = start; i < end; ++i) {
        s[i] = 0;
    }
}

void buffer_newline(buffer_t *buffer) {
    if (buffer->is_one_line) {
        buffer->is_return = true;
        return;
    }
    
    if ((size_t)buffer->cursor_y == buffer->lines->length - 1) {
        stw_vector_push(buffer->lines, calloc(MAX_LINE_LENGTH, 1));
    } else {
        stw_vector_add(buffer->lines, buffer->cursor_y+1, calloc(MAX_LINE_LENGTH, 1));
    }

    buffer->cursor_y++;
    buffer->cursor_x = 0;
}

void buffer_kill_line(buffer_t *buffer) {
    if (buffer->lines->length == 1) return;
    stw_vector_remove(buffer->lines, buffer->cursor_y--);
}

void buffer_remove_previous_word(buffer_t *buffer) {
    if (buffer->cursor_x == 0) return;
    
    char *line = buffer_current_line(buffer);
    char *c = &line[buffer->cursor_x];

    c--;
    buffer->cursor_x--;
    while (*c != ' ' && buffer->cursor_x != 0) {
        buffer_remove_char(buffer, buffer->cursor_x, buffer->cursor_y);
        c--;
        buffer->cursor_x--;
    }
    buffer_remove_char(buffer, buffer->cursor_x, buffer->cursor_y);
}

void buffer_tick(SDL_Event *event, buffer_t *buffer) {
    if (buffer->is_one_line) {
        buffer->is_return = false;
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    switch (event->type) {
    case SDL_TEXTINPUT:
        buffer_type_chars(buffer, event->text.text, buffer->cursor_x);
        break;
    case SDL_KEYDOWN:
        switch (event->key.keysym.sym) {
        case SDLK_RETURN:;
            if (buffer->cursor_x < (int)strlen(buffer_current_line(buffer))) {
                char *after = malloc(strlen(buffer_current_line(buffer)+buffer->cursor_x)+1);
                strcpy(after, buffer_current_line(buffer) + buffer->cursor_x);
                buffer_remove_chars(buffer, buffer->cursor_x, strlen(buffer_current_line(buffer)));
                
                buffer_newline(buffer);
                strcat(buffer_current_line(buffer), after);

                free(after);
            } else {
                buffer_newline(buffer);
            }
            break;
        case SDLK_BACKSPACE:
            if (is_control_held(keys)) {
                buffer_remove_previous_word(buffer);
                break;
            }
            if (buffer->cursor_x == 0 && buffer->cursor_y > 0) {
                char *old_line = buffer_current_line(buffer);
                char *s = malloc(strlen(old_line) + 1);
                strcpy(s, old_line);
                buffer_kill_line(buffer);

                buffer->cursor_x = strlen(buffer_current_line(buffer));

                char *current_line = buffer_current_line(buffer);
                strcat(current_line, s);

                free(old_line);
                free(s);
                break;
            }

            buffer_remove_char(buffer, buffer->cursor_x - 1, buffer->cursor_y);
            buffer->cursor_x--;
            buffer_cursor_limit(buffer);
            break;
        case SDLK_INSERT:
            buffer->overwrite_mode = !buffer->overwrite_mode;
            break;
        case SDLK_d:
            if (is_control_held(keys)) buffer_remove_char(buffer, buffer->cursor_x, buffer->cursor_y);

            break;
        case SDLK_TAB:
            buffer_type_chars(buffer, "    ", buffer->cursor_x);
            break;

        case SDLK_i:
            if (is_control_held(keys)) {
                buffer_cursor_back_indentation(buffer);
            }
            break;

        /* case SDLK_s: */
        /*     if (ibar && is_control_held(keys) && !ibar->is_focused) { */
        /*         ibar->is_focused = true; */
        /*     } */
        /*     break; */

        case SDLK_a:
            if (is_control_held(keys)) buffer->cursor_x = 0;
            break;
        case SDLK_e:
            if (is_control_held(keys)) buffer->cursor_x = strlen(buffer_current_line(buffer));
            break;
        case SDLK_p:
            if (is_control_held(keys)) {
            case SDLK_UP:
                buffer->cursor_y--;   
            }
            buffer_cursor_limit(buffer);
            break;
        case SDLK_n:
            if (is_control_held(keys)) {
            case SDLK_DOWN:
                buffer->cursor_y++;
            }
            buffer_cursor_limit(buffer);
            break;
        case SDLK_b:
            if (is_control_held(keys)) {
            case SDLK_LEFT:
                buffer->cursor_x--;   
                buffer_cursor_limit(buffer);
                break;
            }
            if (is_meta_held(keys)) buffer_cursor_backward_word(buffer);
            
            buffer_cursor_limit(buffer);
            break;
        case SDLK_f:
            if (is_control_held(keys)) {
            case SDLK_RIGHT:
                buffer->cursor_x++;   
                buffer_cursor_limit(buffer);
                break;
            }
            if (is_meta_held(keys)) buffer_cursor_forward_word(buffer);

            buffer_cursor_limit(buffer);
            break;
        case SDLK_k:
            if (is_control_held(keys)) {
                char *s = buffer_current_line(buffer);
                if (s[0]) {
                    buffer_remove_chars(buffer, buffer->cursor_x, strlen(s));
                } else if (buffer->lines->length > 1) {
                    if (buffer->cursor_y < (int)buffer->lines->length - 1) {
                        stw_vector_remove(buffer->lines, buffer->cursor_y);
                    }
                }
            }
            break;
        }
        break;
    }
}

char *buffer_current_line(buffer_t *buffer) {
    return ((char*) buffer->lines->array[buffer->cursor_y]);
}

void buffer_cursor_back_indentation(buffer_t *buffer) {
    buffer->cursor_x = 0;
    char *c = buffer_current_line(buffer);
    
    while (isspace(*c)) {
        c++;
        buffer->cursor_x++;
    }
}

void buffer_cursor_forward_word(buffer_t *buffer) {
    char *line = buffer_current_line(buffer);
    char *c = &line[buffer->cursor_x];
    int line_len = strlen(line);

    c++;
    buffer->cursor_x++;
    while (*c != ' ' && buffer->cursor_x < line_len) {
        c++;
        buffer->cursor_x++;
    }
}
void buffer_cursor_backward_word(buffer_t *buffer) {
    char *line = buffer_current_line(buffer);
    char *c = &line[buffer->cursor_x];

    c--;
    buffer->cursor_x--;
    while (*c != ' ' && buffer->cursor_x != 0) {
        c--;
        buffer->cursor_x--;
    }
}

void buffer_cursor_limit(buffer_t *buffer) {
    if (buffer->cursor_x < 0) buffer->cursor_x = 0;
    if (buffer->cursor_y < 0) buffer->cursor_y = 0;
    if ((size_t)buffer->cursor_y > buffer->lines->length-1) buffer->cursor_y = buffer->lines->length-1;

    char *cl = buffer_current_line(buffer);
    int cl_len = strlen(cl);
    if (buffer->cursor_x > cl_len) buffer->cursor_x = cl_len;
}

void buffer_draw(SDL_Renderer *renderer, buffer_t *buffer) {
    const SDL_Color white = { 255, 255, 255, 255 };

    buffer_cursor_draw(renderer, buffer);
    
    for (size_t i = 0; i < buffer->lines->length; ++i) {
        char *s = buffer->lines->array[i];
        if (s[0] == 0) continue;

        SDL_Surface *surface = TTF_RenderText_Solid(buffer->font, (char*)buffer->lines->array[i], white);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        const SDL_Rect dst = {
            0,
            i * buffer->char_h,
            surface->w,
            surface->h
        };

        SDL_RenderCopy(renderer, texture, NULL, &dst);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

void buffer_cursor_draw(SDL_Renderer *renderer, buffer_t *buffer) {
    const SDL_Rect cursor = {
        buffer->cursor_x * buffer->char_w,
        buffer->cursor_y * buffer->char_h,
        buffer->char_w,
        buffer->char_h
    };

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 0);
    if (buffer->overwrite_mode) {
        SDL_RenderDrawRect(renderer, &cursor);
    } else {
        SDL_RenderFillRect(renderer, &cursor);
    }
}
