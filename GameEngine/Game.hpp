#pragma once
#include <SDL.h>
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

    static void onScriptComponentConstructed(entt::registry& reg, entt::entity self);
    static void onScriptComponentDestroyed(entt::registry& reg, entt::entity self);
    static void onSearchableComponentConstructed(entt::registry& reg, entt::entity entity);
    static void onSearchableComponentDestroyed(entt::registry& reg, entt::entity entity);
    static void invokeCallOnEvent(entt::registry& reg, const SDL_Event& event);
    static void invokeCallOnUpdate(entt::registry& reg);
    static void invokeMovement(entt::registry& reg);
    static void invokeDrawTexture(entt::registry& reg, float interpolation);
    template<typename Tag> static void invokeOnCollision(entt::registry& reg) {
        auto view = reg.view<Tag, PositionComponent, TextureComponent, VelocityComponent, ScriptComponent>();
        auto otherView = reg.view<Tag, PositionComponent, TextureComponent, VelocityComponent>();
        for(entt::entity self : view) {
            const auto& pos = reg.get<PositionComponent>(self);
            const auto& tex = reg.get<TextureComponent>(self);
            const auto& vel = reg.get<VelocityComponent>(self);
            auto& script = reg.get<ScriptComponent>(self);
            for(entt::entity other : otherView) {
                if(self == other) continue;
                const auto& otherPos = reg.get<PositionComponent>(other);
                const auto& otherTex = reg.get<TextureComponent>(other);
                const auto& otherVel = reg.get<VelocityComponent>(other);

                
                if((pos.x <= (otherPos.x + otherTex.width) && (pos.x + tex.width) >= otherPos.x) &&
                   (pos.y <= (otherPos.y + otherTex.height) && (pos.y + tex.height) >= otherPos.y)) {
                    GameObject go = GameObject(reg, self);
                    GameObject otherGo = GameObject(reg, other);
                    script.OnCollision(go, otherGo);
                }
            }
        }
    }

public:
    // Run function to initialize SDL window, renderer, and enter event loop
    static void Run(const Setting& setting, const std::function<void(void)>& onSetup);
    
    // LoadTexture function declaration
    static TextureComponent LoadTexture(const std::string& path);

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
};
