#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void draw_text(SDL_Renderer *renderer, TTF_Font *font, char *text, int x, int y, SDL_Color c)
{
    if (strlen(text) > 0)
    {
        SDL_Surface *text_surface;
        SDL_Color color = c;
        text_surface = TTF_RenderText_Solid(font, text, color);

        SDL_Texture *text_texture;
        text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_Rect dest = {x, y, text_surface->w, text_surface->h};

        SDL_RenderCopy(renderer, text_texture, NULL, &dest);

        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}