#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

typedef struct Save
{
    char path[99];
} Save;

typedef struct Saves
{
    struct Save data[99];
    size_t size;
    char *folder_path;
    int index;
} Saves;

int saves_init(struct Saves *saves)
{

    // Need to be moved but ahah
    saves->folder_path = "./saves/";

    DIR *d;
    struct dirent *dir;
    d = opendir(saves->folder_path);
    int i = 0;
    int index = 0;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (i >= 2)
            {
                strcpy(saves->data[index].path, saves->folder_path);
                strcat(saves->data[index].path, dir->d_name);
                saves->size++;
                index++;

            } // Maybe bad skipping the 2 first but ?
            i++;
        }
        closedir(d);
    }
    return (0);
}

void saves_draw(Saves *saves, SDL_Renderer *renderer, TTF_Font *font, int position_x, int position_y)
{
    SDL_Color color = {255, 0, 0};

    for (int i = 0; i < saves->size; i++)
    {

        draw_text(renderer, font, saves->data[i].path, position_x, position_y + i * 18 - (saves->size + 1) * 18, color);
    }

    draw_text(renderer, font, ">", position_x, position_y + saves->index * 18 - (saves->size + 1) * 18, color);
}

void saves_next(struct Saves *saves)
{

    saves->index++;

    if (saves->index > saves->size-1)
    {
        saves->index = 0;
    }
}