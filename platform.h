#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>

typedef struct platform
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

} platform;

platform *new_platform(char *title, int scaledWidth, int scaledHeight, int unscaledWidth, int unscaledHeight);
void destroy_platform(platform *this);
void update(platform *this, void* buffer, int pitch);
bool process_input(platform *this, uint8_t* keys);