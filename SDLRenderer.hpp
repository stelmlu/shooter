#pragma once
#include <SDL.h>
#include <string>

// RAII wrapper for SDL_Renderer
class SDLRenderer {
public:
    SDLRenderer(SDL_Window* window) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            throw std::runtime_error("Renderer could not be created! SDL Error: " + std::string(SDL_GetError()));
        }
    }

    ~SDLRenderer() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
    }

    SDL_Renderer* get() const { return renderer; }

private:
    SDL_Renderer* renderer = nullptr;
};