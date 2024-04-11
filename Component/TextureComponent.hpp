#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include "../SDLRenderer.hpp"

class TextureComponent {
public:
    TextureComponent(const SDLRenderer& renderer, const std::string& path) : renderer(renderer.get()), path(path) {
        auto& textureInfo = textures[path];
        if (textureInfo.first == nullptr) {
            // Load the texture if it's not already loaded
            textureInfo.first = IMG_LoadTexture(renderer.get(), path.c_str());
            if (!textureInfo.first) {
                throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + std::string(IMG_GetError()));
            }
        }
        // Increment the reference counter
        ++textureInfo.second;
        texture = textureInfo.first;
    }

    // Copy constructor
    TextureComponent(const TextureComponent& other) 
        : renderer(other.renderer), path(other.path), texture(other.texture) {
        // Increment the reference counter for the copied object
        ++textures[path].second;
    }

    // Move constructor
    TextureComponent(TextureComponent&& other) noexcept 
        : renderer(other.renderer), path(std::move(other.path)), texture(other.texture) {
        other.texture = nullptr;
    }

    // Copy assignment operator
    TextureComponent& operator=(const TextureComponent& other) {
        if (this != &other) {
            unloadTexture(); // Unload the current texture if needed
            renderer = other.renderer;
            path = other.path;
            texture = other.texture;
            // Increment the reference counter for the assigned object
            ++textures[path].second;
        }
        return *this;
    }

    // Move assignment operator
    TextureComponent& operator=(TextureComponent&& other) noexcept {
        if (this != &other) {
            unloadTexture(); // Unload the current texture if needed
            renderer = other.renderer;
            path = std::move(other.path);
            texture = other.texture;
            other.texture = nullptr;
        }
        return *this;
    }

    void Draw(float x, float y) const {
        if (texture) {
            SDL_Rect dst;
            int w, h;
            dst.x = static_cast<int>(x);
            dst.y = static_cast<int>(y);
            SDL_QueryTexture(texture, 0, 0, &dst.w, &dst.h);
            SDL_RenderCopy(renderer, texture, NULL, &dst);
        }
    }

    std::pair<int,int> GetSize() const {
        if(texture) {
            int w, h;
            SDL_QueryTexture(texture, 0, 0, &w, &h);
            return {w, h};
        }
        return {0, 0};
    }

    ~TextureComponent() {
        unloadTexture();
    }

    SDL_Texture* get() const { return texture; }

private:
    static std::unordered_map<std::string, std::pair<SDL_Texture*, int>> textures;
    SDL_Renderer* renderer;
    std::string path;
    SDL_Texture* texture = nullptr;

    void unloadTexture() {
        if (texture) {
            auto& textureInfo = textures[path];
            // Decrement the reference counter
            if (--textureInfo.second == 0) {
                // If the counter is 0, destroy the texture and remove it from the map
                SDL_DestroyTexture(textureInfo.first);
                textures.erase(path);
                std::cout << "Destory texture: " << path << "\n";
            }
        }
    }
};
