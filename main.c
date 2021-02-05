#include <stdio.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "stw.h"

#include "buffer.h"
#include "modeline.h"

int screen_width = 720;
int screen_height = 800;

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("tmacs",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          screen_width,
                                          screen_height,
                                          SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    bool running = true;

    TTF_Font *font = TTF_OpenFont("cour.ttf", 16);

    buffer_t *buf = buffer_make(font);
    modeline_t *modeline = modeline_make(font);

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                screen_width = event.window.data1;
                screen_height = event.window.data2;
            }

            buffer_tick(&event, buf);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            buffer_draw(renderer, buf);
            modeline_draw(renderer, modeline, screen_width, screen_height);
            
            SDL_RenderPresent(renderer);
        }
    }

    buffer_free(buf);
    modeline_free(modeline);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    
    SDL_Quit();
    TTF_Quit();

    return 0;
}
