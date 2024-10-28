#include "platform.h"

platform *new_platform(char *title, int width, int height)
{

    platform *platform = malloc(sizeof(*platform));
    SDL_CreateWindowAndRenderer(width, height, 0, &platform->window, &platform->renderer);
    SDL_SetWindowTitle(platform->window, title);

    return platform;
}

void delete_platform(platform *platform)
{
    SDL_DestroyTexture(platform->texture);
    SDL_DestroyRenderer(platform->renderer);
    SDL_DestroyWindow(platform->window);
    free(platform);
}