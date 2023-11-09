#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "draw_utils.c"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define LIST_LENGTH 20
#define MAX_STRING_LENGTH 100

struct Item
{
    char text[MAX_STRING_LENGTH];
    int statue;
};

struct List
{
    struct Item *items;
    size_t size;
    int index;
    int cursor_index;
    char *save_path;
};

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

int list_init(struct List *list)
{
    list->save_path = "./saves/save.txt";

    list->items = calloc(list->size, sizeof(struct Item));
    if (list->items == NULL)
    {
        return 1;
    }
    return 0;
}

int list_set_size(struct List *list, size_t new_size)
{

    list->size = new_size;
    list->items = realloc(list->items, list->size * sizeof(struct Item));
    if (list->items == NULL)
    {
        return 1;
    }
    return 0;
}

int list_free(struct List *list)
{
    free(list->items);
}

void list_draw(struct List *list, SDL_Renderer *renderer, TTF_Font *font, int position_x, int position_y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Color color = {255, 255, 255};

    for (int i = 0; i < list->size; i++)
    {

        int width;
        int height;
        TTF_SizeText(font, list->items[i].text, &width, &height);

        int x = position_x;
        int y = position_y + i * height;

        char *line_indicator;
        if (list->index == i)
        {
            line_indicator = ">";
            x += 5;
        }
        else
        {
            if (list->items[i].statue)
            {
                line_indicator = "O";
            }
            else
            {
                line_indicator = "X";
            }
        }
        int line_indicator_width;
        TTF_SizeText(font, line_indicator, &line_indicator_width, NULL);
        draw_text(renderer, font, line_indicator, x + line_indicator_width, y, color);

        int nx = x + line_indicator_width * 2 + 8;

        if (list->items[i].statue && strlen(list->items[i].text))
        {
            int text_size = strlen(list->items[i].text);
            SDL_Rect rect = {nx, y + height / 2 - 1, width, 2 };
            SDL_RenderDrawRect(renderer, &rect);
        }

        char *text = list->items[i].text;
        SDL_Color red_color = {255, 0, 0};
        draw_text(renderer, font, text, nx, y, color);
    }
}

void list_save(struct List *list)
{
    FILE *fptr;

    fptr = fopen(list->save_path, "w");

    char item_statue_buffer[5];

    for (int i = 0; i < list->size; i++)
    {
        if (strlen(list->items[i].text) == 0)
        {
            continue;
        }

        itoa(list->items[i].statue, item_statue_buffer, 10);

        char *item_text = list->items[i].text;
        fputs(item_text, fptr);
        fputc('|', fptr);
        fputs(item_statue_buffer, fptr);
        putc('\n', fptr);
    }

    fclose(fptr);
}

int list_load(struct List *list)
{

    FILE *file;
    file = fopen(list->save_path, "r");

    if (file == NULL)
    {
        return 1;
    }

    char current_line[999];

    while (fgets(current_line, 999, file))
    {
        list_set_size(list, list->size + 1); // May check if the list is not null
        struct Item new_item;
        sscanf(current_line, "%[^|];%d", new_item.text, &new_item.statue);
        list->items[list->size - 1] = new_item;
    }

    fclose(file);

    return 0;
}

void list_current_item_add_character(struct List *list, char c)
{

    size_t size = strlen(list->items[list->index].text);

    if (size + 1 < MAX_STRING_LENGTH)
    {
        list->items[list->index].text[size] = c;
        list->items[list->index].text[size + 1] = '\0';
    }
}

void list_current_item_remove_character(struct List *list)
{
    size_t size = strlen(list->items[list->index].text) - 1;

    if (size >= 0)
    {
        list->items[list->index].text[size] = '\0';
        list->items[list->index].text[size + 1] = '\0';
    }
}

void list_current_item_toggle_statue(struct List *list)
{
    list->items[list->index].statue = !list->items[list->index].statue;
}

void list_next_item(struct List *list)
{
    if (list->index == list->size)
    {
        list_set_size(list, list->size + 1); // May check if the list is not null

        struct Item new_item = {"", 0};
        list->items[list->size - 1] = new_item;
    }
    list->index++;
}

void list_previous_item(struct List *list)
{
    if (list->index > 0)
        list->index--;
}
