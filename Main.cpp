#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <entity/registry.hpp>
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
#include "Component/TextureEntityComponent.hpp"
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

entt::entity LoadTexture(entt::registry &reg, const SDLRenderer& renderer, const std::string& path) {
    // Search for the entity with a PathComponent and TextureComponent and return if found.
    auto view = reg.view<PathComponent, TextureComponent>();
    for(auto [self, pathcomp, texture]: view.each()) {
        if(pathcomp.path == path) return self;
    }

    // Not found, then create a new and load the texture and add Path and TextureComponent.
    const entt::entity self = reg.create();
    SDL_Texture* texptr = IMG_LoadTexture(renderer.get(), path.c_str());
    if (!texptr) {
        throw std::runtime_error("Unable to load image " + path + "! SDL_image Error: " + std::string(IMG_GetError()));
    }
    std::cout << "Load texture: " << path << "\n";
    int width, height;
    SDL_QueryTexture(texptr, 0, 0, &width, &height);
    reg.emplace<TextureComponent>(self, texptr, width, height);
    reg.emplace<PathComponent>(self, path);
    return self;
}

entt::entity FindTexture(entt::registry &reg, const std::string& path) {
    // Search for the entity with a PathComponent and TextureComponent and return if found.
    auto view = reg.view<PathComponent, TextureComponent>();
    for(auto [self, pathcomp, texture]: view.each()) {
        if(pathcomp.path == path) return self;
    }
    throw std::runtime_error("Unable to find texture for " + path + "!");
}

entt::entity CreatePlayerBullet(entt::registry& reg) {
    const entt::entity id = reg.create();
    reg.emplace<PositionComponent>(id, 0.0f, 100.0f);
    reg.emplace<VelocityComponent>(id, PLAYER_BULLET_SPEED, 0.0f);
    reg.emplace<KeyStateComponent>(id);
    reg.emplace<TextureEntityComponent>(id, FindTexture(reg, "gfx/playerBullet.png"));
    reg.emplace<ScriptComponent>(id, DestroyWhenLeavningScreenScript{});
    return id;
}

entt::entity CreatePlayer(entt::registry& reg) {
    const entt::entity id = reg.create();
    reg.emplace<PositionComponent>(id, 100.0f, 100.0f);
    reg.emplace<VelocityComponent>(id, 0.0f, 0.0f);
    reg.emplace<KeyStateComponent>(id);
    reg.emplace<TextureEntityComponent>(id, FindTexture(reg, "gfx/player.png"));
    reg.emplace<ScriptComponent>(id, PlayerProxyScript{ CreatePlayerBullet });
    return id;
}

entt::registry CreatePlayground(const SDLRenderer& renderer) {
    auto reg = CreateRegistry();
    LoadTexture(reg, renderer, "gfx/player.png");
    LoadTexture(reg, renderer, "gfx/playerBullet.png");
    const entt::entity player = CreatePlayer(reg);
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
    auto view = reg.view<const PositionComponent, const TextureEntityComponent>();
    view.each([&reg,&renderer](const auto& pos, const auto& texentity) {
        auto& tex = reg.get<TextureComponent>(texentity.entity);
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
