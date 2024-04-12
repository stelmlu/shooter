#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "../SDLRenderer.hpp"

struct TextureComponent {
    static std::unordered_map<std::string, SDL_Texture*> textures;
public:
    SDL_Texture* texture = nullptr;
    int width, height;

    TextureComponent(const SDLRenderer& renderer, const std::string& path): width(-1), height(-1) {
        texture = textures[path];
        if (texture == nullptr) {
            // Load the texture if it's not already loaded
            texture = IMG_LoadTexture(renderer.get(), path.c_str());
            if (!texture) {
                throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + std::string(IMG_GetError()));
            }
            std::cout << "Load texture: " << path << " " << texture << "\n";
            textures[path] = texture;
        }
        SDL_QueryTexture(texture, 0, 0, &width, &height);
    }
};
