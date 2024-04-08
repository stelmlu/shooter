#pragma once
#include <SDL.h>
#include <string>

// RAII wrapper for SDL_Window
class SDLWindow {
public:
    SDLWindow(const char* title, int width, int height) {
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
        if (!window) {
            throw std::runtime_error("Window could not be created! SDL Error: " + std::string(SDL_GetError()));
        }
    }

    ~SDLWindow() {
        if (window) {
            SDL_DestroyWindow(window);
        }
    }

    SDL_Window* get() const { return window; }

private:
    SDL_Window* window = nullptr;
};