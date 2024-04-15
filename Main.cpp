#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <entity/registry.hpp>
#include <random>
#include "Settings.hpp"
#include "SDLSystem.hpp"
#include "SDLWindow.hpp"
#include "SDLRenderer.hpp"
#include "Component/PositionComponent.hpp"
#include "Component/ScriptComponent.hpp"
#include "Component/VelocityComponent.hpp"
#include "Component/KeyStateComponent.hpp"
#include "Component/PathComponent.hpp"
#include "Component/TextureComponent.hpp"
#include "Script/PlayerMoveInputScript.hpp"
#include "Script/PlayerProxyScript.hpp"
#include "Script/DestroyWhenLeavingScreenScript.hpp"

void OnScriptComponentConstructed(entt::registry& reg, entt::entity self) {
    reg.get<ScriptComponent>(self).OnConstructed(reg, self);
}

void OnScriptComponentDestroyed(entt::registry& reg, entt::entity self) {
    reg.get<ScriptComponent>(self).OnDestroyed(reg, self);
}

entt::registry CreateRegistry() {
    entt::registry reg;
    reg.on_construct<ScriptComponent>().connect<&OnScriptComponentConstructed>();
    reg.on_destroy<ScriptComponent>().connect<&OnScriptComponentDestroyed>();
    return reg;
}

static std::unordered_map<std::string, TextureComponent> textureComponentMap;

const TextureComponent& LoadTexture(const SDLRenderer& renderer, const std::string& path) {
    // Search for the entity with a PathComponent and TextureComponent and return if found.
    auto& textureComponent = textureComponentMap[path];
    if(textureComponent.texture == nullptr) {
        // Not found, then create a new and load the texture and add Path and TextureComponent.
        textureComponent.texture = IMG_LoadTexture(renderer.get(), path.c_str());
        if (!textureComponent.texture) {
            throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + std::string(IMG_GetError()));
        }
        SDL_QueryTexture(textureComponent.texture, 0, 0, &textureComponent.width, &textureComponent.height);
        std::cout << "Load texture: " << path << "\n";
    }
    return textureComponent;
}

const TextureComponent& FindTexture(entt::registry &reg, const std::string& path) {
    const auto& textureComponent = textureComponentMap[path];
    if(textureComponent.texture == nullptr) {
        throw std::runtime_error("Unable to find texture for " + path + "!");
    }
    return textureComponent;
}

namespace std {
    template<> struct hash<pair<float, float>> {
        size_t operator()(const pair<float, float>& p) const noexcept {
            return hash<float>{}(p.first) ^ (hash<float>{}(p.second) << 1);
        }
    };
}

static std::unordered_map<std::pair<float,float>, std::uniform_real_distribution<float>> uniformRealDistMap;

float GenerateRandomNumber(float from, float to) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto key = std::make_pair(from, to);
    auto findResult = uniformRealDistMap.find(std::make_pair(from, to));
    if(findResult == uniformRealDistMap.end()) {
        uniformRealDistMap.emplace(std::make_pair(key, std::uniform_real_distribution<float>(from, to)));
        return uniformRealDistMap[key](gen);
    }
    return findResult->second(gen);
}

entt::entity CreatePlayerBullet(entt::registry& reg) {
    const entt::entity id = reg.create();
    reg.emplace<PositionComponent>(id, 0.0f, 100.0f);
    reg.emplace<VelocityComponent>(id, PLAYER_BULLET_SPEED, 0.0f);
    reg.emplace<KeyStateComponent>(id);
    reg.emplace<TextureComponent>(id, FindTexture(reg, "gfx/playerBullet.png"));
    reg.emplace<ScriptComponent>(id, DestroyWhenLeavningScreenScript{});
    return id;
}

entt::entity CreatePlayer(entt::registry& reg) {
    const entt::entity id = reg.create();
    reg.emplace<PositionComponent>(id, 100.0f, 100.0f);
    reg.emplace<VelocityComponent>(id, 0.0f, 0.0f);
    reg.emplace<KeyStateComponent>(id);
    reg.emplace<TextureComponent>(id, FindTexture(reg, "gfx/player.png"));
    reg.emplace<ScriptComponent>(id, PlayerProxyScript{ CreatePlayerBullet });
    return id;
}

entt::entity CreateEnemy(entt::registry& reg) {
    const entt::entity self = reg.create();
    reg.emplace<PositionComponent>(self, SCREEN_WIDTH + 50.0f, GenerateRandomNumber(0.0f, SCREEN_HEIGHT - 48.0f));
    auto speed = -GenerateRandomNumber(100.0f, 200.0f);
    reg.emplace<VelocityComponent>(self, -GenerateRandomNumber(100.0f, 200.0f), 0.0f);
    reg.emplace<TextureComponent>(self, FindTexture(reg, "gfx/enemy.png"));
    return self;
}

entt::registry CreatePlayground(const SDLRenderer& renderer) {
    LoadTexture(renderer, "gfx/player.png");
    LoadTexture(renderer, "gfx/playerBullet.png");
    LoadTexture(renderer, "gfx/enemy.png");

    auto reg = CreateRegistry();
    CreatePlayer(reg);
    return reg;
}

void InvokeCallOnEvent(entt::registry& reg, const SDL_Event& event) {
    auto view = reg.view<ScriptComponent>();
    view.each([&reg, &event](entt::entity self, auto& script) {
        script.OnEvent(reg, self, event);
    });
}

void InvokeCallOnUpdate(entt::registry& reg) {
    auto view = reg.view<ScriptComponent>();
    view.each([&reg](entt::entity self, auto& script) {
        script.OnUpdate(reg, self, SECOND_PER_UPDATE);
    });
}

void InvokeMovement(entt::registry& reg) {
    auto view = reg.view<PositionComponent, const VelocityComponent>();
    view.each([](auto& pos, const auto& vel){
        pos.x += vel.dx * SECOND_PER_UPDATE;
        pos.y += vel.dy * SECOND_PER_UPDATE;
    });
}

void InvokeDrawTexture(entt::registry& reg, const SDLRenderer& renderer, float interpolation) {
    auto view = reg.view<const PositionComponent, const TextureComponent>();
    view.each([&reg,&renderer](const auto& pos, const auto& tex) {
        SDL_Rect dst = {
            static_cast<int>(pos.x + 0.5f), static_cast<int>(pos.y + 0.5f),
            tex.width, tex.height
        };
        SDL_RenderCopy(renderer.get(), tex.texture, NULL, &dst);
    });
}

int main() {
    try {
        SDLSystem sdlSystem;
        SDLWindow window("Shooter", SCREEN_WIDTH, SCREEN_HEIGHT);
        SDLRenderer renderer(window.get());

        entt::registry reg = CreatePlayground(renderer);

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
                InvokeCallOnEvent(reg, event);
            }

            while(lag >= ms_per_update) {
                InvokeCallOnUpdate(reg);
                InvokeMovement(reg);
                lag -= ms_per_update;
            }

            SDL_RenderClear(renderer.get());
            InvokeDrawTexture(reg, renderer, lag / ms_per_update);
            SDL_RenderPresent(renderer.get());

            SDL_Delay(16);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
