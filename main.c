#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "text.c"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define LIST_LENGTH 20
#define MAX_STRING_LENGTH 128

struct Item
{
    char text[MAX_STRING_LENGTH];
    int statue;
};

struct List
{
    struct Item* items;
    size_t size;
    int index;
    int cursor_index;
};

struct List list;

TTF_Font *font;

/*
    TODO:
    - Manage errors                     [ ]
    - Make the list infinit             [X]
    - Refactor the code                 [ ]
    - Save the list in file             [X]
    - Open list from file               [X]
    - Make a better key system          [KINDA]
    - Refactor the save and load code   [ ]
    - Allow to use the cursor for text  [ ]
    - Show save states                  [ ]
*/

int init_list() {

    list.items = calloc(list.size, sizeof(struct Item));
    if (list.items == NULL) {
        return 1;
    }
    return 0;
}

int set_list_size(size_t new_size) {
    
    list.size = new_size;
    list.items = realloc(list.items, list.size * sizeof(struct Item));
    if (list.items == NULL) {
        return 1;
    }
    return 0;
}

int free_list() {

    free(list.items);
}

void draw_list(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Color color = {255, 255, 255};

    for (int i = 0; i < list.size; i++)
    {

        int x = 16;
        int y = 4 + i * 18;

        if (list.items[i].statue)
        {
            int text_size = strlen(list.items[i].text);
            if (text_size > 0)
                SDL_RenderDrawLine(renderer, x, y + 7, x + text_size * 9, y + 7);
        }

        char *text = list.items[i].text;
        SDL_Color red_color = {255, 0, 0};
        draw_text(renderer, font, text, x, y, color);

        if (list.index == i)
            continue;

        draw_text(renderer, font, "-", x - 12, y, color);
    }
    SDL_Color white_color = {255, 255, 255};
    draw_text(renderer, font, ">", 4, 4 + list.index * 18, color);

    //TODO : MAKE THIS NOT USELESS
    //draw_text(renderer, font, "|", 4 + list.cursor_index * 9, list.index * 18, color);
}

void save_list()
{
    FILE *fptr;

    fptr = fopen("./saves/save.txt", "w");

    char buffer[5];

    for (int i = 0; i < list.size; i++)
    {

        itoa(list.items[i].statue, buffer, 10);

        char *item_text = list.items[i].text;
        char *item_statue = buffer;
        fputs(item_text, fptr);
        fputc('|', fptr);
        fputs(item_statue, fptr);
        putc('\n', fptr);
    }

    fclose(fptr);
}

void load_list()
{

    FILE *fptr;
    fptr = fopen("./saves/save.txt", "r");

    char c;
    int buffer_index = 0;
    int index = 0;

    if (fptr)
    {
        printf("Save loaded !");
    }
    else
    {
        printf("No 'save.txt' file ...");
    }

    while (c != EOF)
    {
        c = getc(fptr);

        if (c == '|')
        {
            //Update the size of the list 
            set_list_size( list.size + 1);

            c = getc(fptr);
            const char buffer[1] = {c};
            list.items[buffer_index].statue = atoi(buffer);
            c = getc(fptr);
            buffer_index++;
            index = 0;
        }
        else
        {
            if (index < MAX_STRING_LENGTH) {
                list.items[buffer_index].text[index] = c;
            }
            index++;
        }
    }

    fclose(fptr);
}

void add_character(char c)
{

    size_t size = strlen(list.items[list.index].text);

    if (size < 128)
    {
        list.items[list.index].text[size] = c;
        list.items[list.index].text[size + 1] = '\0';
    }
}

void remove_character()
{
    size_t size = strlen(list.items[list.index].text) - 1;

    if (size >= 0)
    {
        list.items[list.index].text[size] = '\0';
        list.items[list.index].text[size + 1] = '\0';
    }
}

void toggle_item_statue()
{
    list.items[list.index].statue = !list.items[list.index].statue;
}

void next_item()
{
    if (list.index > list.size - 1) {
        set_list_size(list.size + 1);
    }
    list.index++;
}

void previous_item()
{
    if (list.index > 0)
        list.index--;
}

void move_cursor_left()
{

    if (list.cursor_index >= 0)
    {
        list.cursor_index--;
    }
}

void move_cursor_right()
{

    if (list.cursor_index <= MAX_STRING_LENGTH)
    {
        list.cursor_index++;
    }
}

void manage_key(SDL_Keycode key_code)
{
    switch (key_code)
    {
    case SDLK_DOWN:
        next_item();
        break;
    case SDLK_UP:
        previous_item();
        break;
    case SDLK_RETURN:
        toggle_item_statue();
        break;
    case SDLK_LSHIFT:
        save_list();
        break;
    case SDLK_BACKSPACE:
        remove_character();
        break;
    default:
        add_character((char)key_code);
        break;
    }
}

int WinMain(int argc, char *argv[])
{
    SDL_Init(0);
    TTF_Init();
    SDL_Window *window = SDL_CreateWindow("TO-DO", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    int quit = 0;
    SDL_Event e;

    list.size = 1;

    init_list();

    load_list();

    // Load font
    font = TTF_OpenFont("./assets/fonts/font.ttf", 16);

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

    free_list();

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}