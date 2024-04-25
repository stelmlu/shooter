#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include <unordered_map>
#include <random>
#include <entity/registry.hpp>
#include "GameEngine.hpp"

// Add to the game clear EnTT registry and call onApply function
template<typename Func>
static void AddToGame(Func onApply, bool reset = false) {
    if (reset) Registry::Get().clear();
    onApply();
}

class Game {
    static SDL_Window* m_window;
    static SDL_Renderer* m_renderer;
    static std::unordered_map<std::string, SDL_Texture*> m_textureCache;
    static float m_secondPerFrame;
    static std::unordered_map<std::string, entt::entity> m_searchableMap;

    static std::random_device m_randomDevice;
    static std::mt19937 m_radomGenerator;
    static std::map<std::pair<float, float>, std::uniform_real_distribution<float>> m_randomDistributions;

    static std::unordered_map<std::string, TextureComponent> m_textureRectCache;

    static void onScriptComponentConstructed(entt::registry& reg, entt::entity self);
    static void onScriptComponentDestroyed(entt::registry& reg, entt::entity self);
    static void onSearchableComponentConstructed(entt::registry& reg, entt::entity entity);
    static void onSearchableComponentDestroyed(entt::registry& reg, entt::entity entity);

public:
    // Run function to initialize SDL window, renderer, and enter event loop
    static void Run(Setting& setting, const std::function<void(void)>& onSetup);

    // Random Generator
    static float GenerateRandom(float from, float to) {
        std::pair<float, float> key = {from, to};

        // Check if the distribution for this range already exists
        if (m_randomDistributions.find(key) == m_randomDistributions.end()) {
            // If not, create and store it
            m_randomDistributions[key] = std::uniform_real_distribution<float>(from, to);
        }

        // Use the distribution to generate a random float
        return m_randomDistributions[key](m_radomGenerator);
    }

    // Find game object that has searchable component
    static GameObject FindGameObject(const std::string& name) {
        auto findResult = m_searchableMap.find(name);
        if(findResult == m_searchableMap.end()) return GameObject(Registry::Get(), entt::null);
        return GameObject(Registry::Get(), findResult->second);
    }

    static TextureComponent LoadTexture(const std::string& path) {
        if (m_textureRectCache.find(path) != m_textureRectCache.end()) {
            return TextureComponent{ m_textureRectCache[path] };
        } else {
            auto textureComponent = TextureComponent{ IMG_LoadTexture(m_renderer, path.c_str()) };
            int w, h;
            SDL_QueryTexture(textureComponent.texture, 0, 0, &w, &h);
            textureComponent.width = static_cast<float>(w);
            textureComponent.height = static_cast<float>(h);
            m_textureRectCache[path] = textureComponent;
            return textureComponent;
        }
    }
};
