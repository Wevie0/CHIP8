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

platform *new_platform(char* title, int width, int height);
void delete_platform(platform *platform);
void update(platform *platform);
bool process_input(platform *platform);