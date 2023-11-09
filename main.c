#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "todo.c"

TTF_Font *font;
struct List list;

void manage_key(SDL_Keycode key_code)
{
    switch (key_code)
    {
    case SDLK_DOWN:
        list_next_item(&list);
        break;
    case SDLK_UP:
        list_previous_item(&list);
        break;
    case SDLK_RETURN:
        list_current_item_toggle_statue(&list);
        break;
    case SDLK_LSHIFT:
        list_save(&list);
        break;
    case SDLK_BACKSPACE:
        list_current_item_remove_character(&list);
        break;
    default:
       list_current_item_add_character(&list,(char)key_code);
        break;
    }
}

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

    font = TTF_OpenFont("./assets/fonts/font.ttf", 16);
    if (font == NULL) {
        printf("Can't load font");
        return 1;
    }

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
    list.size = 0;
    if (list_init(&list)) {
        printf("Can't init the list");
        return 1;
    }
    
    if (list_load(&list)) {
        printf("Can't load the list from save");
        return 1;
    };

    while (!quit)
    {
        SDL_RenderClear(renderer);

        list_draw(&list, renderer, font, 16, 16);

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
    list_free(&list);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}