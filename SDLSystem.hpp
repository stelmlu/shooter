#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>

// RAII wrapper for SDL
class SDLSystem {
public:
    SDLSystem() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error("SDL could not initialize! SDL Error: " + std::string(SDL_GetError()));
        }

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            throw std::runtime_error("SDL_image could not initialize! SDL_image Error: " + std::string(IMG_GetError()));
        }
    }

    ~SDLSystem() {
        IMG_Quit();
        SDL_Quit();
    }
};
