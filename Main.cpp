#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "Settings.hpp"
#include "SDLSystem.hpp"
#include "SDLWindow.hpp"
#include "SDLRenderer.hpp"

int main() {
    try {
        SDLSystem sdlSystem;
        SDLWindow window("Shooter", SCREEN_WIDTH, SCREEN_HEIGHT);
        SDLRenderer renderer(window.get());

        bool quit = false;
        SDL_Event event;


        float ms_per_update = SECOND_PER_UPDATE * 1000.0f;
        float previous = static_cast<float>(SDL_GetTicks64());
        float lag = 0.0f;
        while (!quit) {
            float current = static_cast<float>(SDL_GetTicks64());
            float elapsed = current - previous;
            previous = current;
            lag += elapsed;

            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                // InvokeCallOnEvent(world, event);
            }

            while(lag >= ms_per_update) {
                // InvokeCallOnUpdate(world);
                // InvokeMovement(world);
                lag -= ms_per_update;
            }

            SDL_RenderClear(renderer.get());
            // InvokeDrawTexture(world, lag / ms_per_update);
            SDL_RenderPresent(renderer.get());

            SDL_Delay(16);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

// Static member definition
// std::unordered_map<std::string, std::pair<SDL_Texture*, int>> TextureComponent::textures;
