#include "platform.h"

platform *new_platform(char *title, int scaledWidth, int scaledHeight,
                       int unscaledWidth, int unscaledHeight) {

  platform *platform = malloc(sizeof(*platform));

  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_CreateWindowAndRenderer(scaledWidth, scaledHeight, 0, &platform->window,
                              &platform->renderer);
  SDL_SetWindowTitle(platform->window, title);

  platform->texture = SDL_CreateTexture(
      platform->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
      unscaledWidth, unscaledHeight);

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

    default:
      break;
    }
  }
  return quit;
}