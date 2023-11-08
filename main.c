#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "todo.c"

int WinMain(int argc, char *argv[])
{
    int sdl_init = SDL_Init(0);
    if (sdl_init) {
        printf("SDL can't be initalize");
        return 1;
    }

    int sdl_tff_int =  TTF_Init();
    if (sdl_init) {
        printf("SDL_TTF can't be initalize");
        return 1;
    }
    // Load font
    font = TTF_OpenFont("./assets/fonts/font.ttf", 16);

    SDL_Window *window = SDL_CreateWindow("TO-DO", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        printf("SDL_Window can't be initalize");
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL) {
        printf("SDL_Render can't be initalize");
        return 1;
    }

    int quit = 0;

    SDL_Event e;

    //Initalize the to-do list
    list.size = 1;
    if (init_list()) {
        printf("Can't init the list");
        return 1;
    }
    
    if (load_list()) {
        printf("Can't load the list from save");
        return 1;
    };

    while (!quit)
    {
        SDL_RenderClear(renderer);

        draw_list(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = 1;
            }

            if (e.type == SDL_KEYDOWN)
            {
                manage_key(e.key.keysym.sym);
            }
        }

        SDL_Delay(1000 / 60);
    }

    //Free the to-do list
    free_list();

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}