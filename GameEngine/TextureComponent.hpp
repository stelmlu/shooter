#pragma once
#include <SDL.h>

struct TextureComponent {
    SDL_Texture* texture = nullptr;
    int width, height;
};
