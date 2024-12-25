#include "platform.h"
#include <stdbool.h>

platform *new_platform(char *title, int scaledWidth, int scaledHeight,
                       int unscaledWidth, int unscaledHeight) {

    platform *platform = malloc(sizeof(*platform));

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(scaledWidth, scaledHeight, 0, &platform->window,
                                &platform->renderer);
    SDL_SetWindowTitle(platform->window, title);

    platform->texture = SDL_CreateTexture(
        platform->renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, unscaledWidth, unscaledHeight);

    return platform;
}

void destroy_platform(platform *this) {
    SDL_DestroyTexture(this->texture);
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    free(this);
}

void update(platform *this, void *buffer, int pitch) {
    SDL_UpdateTexture(this->texture, NULL, buffer, pitch);
    SDL_RenderClear(this->renderer);
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
    SDL_RenderPresent(this->renderer);
}

bool process_input(platform *this, uint8_t *keys) {
    bool quit = false;

    SDL_Event evt;

    while (SDL_PollEvent(&evt)) {
        switch (evt.type) {
        case SDL_QUIT:
            quit = true;
            break;

        case SDL_KEYDOWN: {
            switch (evt.key.keysym.sym) {
            case SDLK_ESCAPE: {
                quit = true;
            } break;

            case SDLK_x: {
                keys[0] = 1;
            } break;

            case SDLK_1: {
                keys[1] = 1;
            } break;

            case SDLK_2: {
                keys[2] = 1;
            } break;

            case SDLK_3: {
                keys[3] = 1;
            } break;

            case SDLK_q: {
                keys[4] = 1;
            } break;

            case SDLK_w: {
                keys[5] = 1;
            } break;

            case SDLK_e: {
                keys[6] = 1;
            } break;

            case SDLK_a: {
                keys[7] = 1;
            } break;

            case SDLK_s: {
                keys[8] = 1;
            } break;

            case SDLK_d: {
                keys[9] = 1;
            } break;

            case SDLK_z: {
                keys[0xA] = 1;
            } break;

            case SDLK_c: {
                keys[0xB] = 1;
            } break;

            case SDLK_4: {
                keys[0xC] = 1;
            } break;

            case SDLK_r: {
                keys[0xD] = 1;
            } break;

            case SDLK_f: {
                keys[0xE] = 1;
            } break;

            case SDLK_v: {
                keys[0xF] = 1;
            } break;
            }
        } break;

        case SDL_KEYUP: {
            switch (evt.key.keysym.sym) {
            case SDLK_x: {
                keys[0] = 0;
            } break;

            case SDLK_1: {
                keys[1] = 0;
            } break;

            case SDLK_2: {
                keys[2] = 0;
            } break;

            case SDLK_3: {
                keys[3] = 0;
            } break;

            case SDLK_q: {
                keys[4] = 0;
            } break;

            case SDLK_w: {
                keys[5] = 0;
            } break;

            case SDLK_e: {
                keys[6] = 0;
            } break;

            case SDLK_a: {
                keys[7] = 0;
            } break;

            case SDLK_s: {
                keys[8] = 0;
            } break;

            case SDLK_d: {
                keys[9] = 0;
            } break;
            case SDLK_z: {
                keys[0xA] = 0;
            } break;

            case SDLK_c: {
                keys[0xB] = 0;
            } break;

            case SDLK_4: {
                keys[0xC] = 0;
            } break;

            case SDLK_r: {
                keys[0xD] = 0;
            } break;

            case SDLK_f: {
                keys[0xE] = 0;
            } break;

            case SDLK_v: {
                keys[0xF] = 0;
            } break;
            }
        } break;

        default:
            break;
        }
    }
    return quit;
}
