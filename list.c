#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "draw_utils.c"
#include "files.c"

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

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define LIST_LENGTH 20
#define MAX_STRING_LENGTH 100

typedef struct Item
{
    char text[MAX_STRING_LENGTH];
    int statue;
} Item;

typedef struct List
{
    struct Item *data;
    size_t size;
    int index;
    int cursor_index;
    int saved;
} List;

int list_init(List *list)
{
    // list_get_saves(list);

    list->data = calloc(list->size, sizeof(Item));
    if (list->data == NULL)
    {
        return 1;
    }
    return 0;
}

int list_set_size(struct List *list, size_t new_size)
{

    list->size = new_size;
    list->data = realloc(list->data, list->size * sizeof(Item));
    if (list->data == NULL)
    {
        return 1;
    }
    return 0;
}

int list_free(List *list)
{
    free(list->data);
}

void list_draw(List *list, SDL_Renderer *renderer, TTF_Font *font, int position_x, int position_y)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Color color = {255, 255, 255};

    char *list_is_saved;
    if (list->saved) {
        list_is_saved = "Saved";
    } else {
        list_is_saved = "Not saved";
    }

    draw_text(renderer, font, list_is_saved ,position_x, position_y, color);

    for (int i = 0; i < list->size; i++)
    {

        int width;
        int height;
        TTF_SizeText(font, list->data[i].text, &width, &height);

        int x = position_x;
        int y = position_y + i * height + 18;

        char *line_indicator;
        if (list->index == i)
        {
            line_indicator = ">";
            x += 5;
        }
        else
        {
            if (list->data[i].statue)
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

        if (list->data[i].statue && strlen(list->data[i].text))
        {
            int text_size = strlen(list->data[i].text);
            SDL_Rect rect = {nx, y + height / 2 - 1, width, 2};
            SDL_RenderDrawRect(renderer, &rect);
        }

        char *text = list->data[i].text;
        SDL_Color red_color = {255, 0, 0};
        draw_text(renderer, font, text, nx, y, color);
    }
}

void list_save(List *list, struct Saves *saves)
{
    FILE *fptr;

    fptr = fopen(saves->data[saves->index].path, "w");

    char item_statue_buffer[5];

    for (int i = 0; i < list->size; i++)
    {
        if (strlen(list->data[i].text) == 0)
        {
            continue;
        }

        itoa(list->data[i].statue, item_statue_buffer, 10);

        char *item_text = list->data[i].text;
        fputs(item_text, fptr);
        fputc('|', fptr);
        fputs(item_statue_buffer, fptr);
        putc('\n', fptr);
    }

    fclose(fptr);

    list->saved = 1;
}

int list_load(List *list, struct Saves *saves)
{

    FILE *file;
    file = fopen(saves->data[saves->index].path, "r");

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
        list->data[list->size - 1] = new_item;
    }

    fclose(file);

    return 0;
}

void list_current_item_add_character(List *list, char c)
{

    size_t size = strlen(list->data[list->index].text);

    if (size + 1 < MAX_STRING_LENGTH)
    {
        list->data[list->index].text[size] = c;
        list->data[list->index].text[size + 1] = '\0';
    }

    list->saved = 0;
}

void list_current_item_remove_character(List *list)
{
    size_t size = strlen(list->data[list->index].text) - 1;

    if (size >= 0)
    {
        list->data[list->index].text[size] = '\0';
        list->data[list->index].text[size + 1] = '\0';
    }

    list->saved = 0;
}

void list_current_item_toggle_statue(List *list)
{
    list->data[list->index].statue = !list->data[list->index].statue;
}

void list_next_item(List *list)
{
    if (list->index == list->size)
    {
        list_set_size(list, list->size + 1); // May check if the list is not null

        struct Item new_item = {"", 0};
        list->data[list->size - 1] = new_item;
    }
    list->index++;
}

void list_previous_item(List *list)
{
    if (list->index > 0)
        list->index--;
}
