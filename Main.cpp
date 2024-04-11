#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "Loom.hpp"
#include "Settings.hpp"
#include "SDLSystem.hpp"
#include "SDLWindow.hpp"
#include "SDLRenderer.hpp"
#include "Component/PositionComponent.hpp"
#include "Component/TextureComponent.hpp"
#include "Component/ScriptComponent.hpp"
#include "Component/PriorityQueueComponent.hpp"
#include "Component/VelocityComponent.hpp"
#include "Component/KeyStateComponent.hpp"
#include "Component/PriorityQueueComponent.hpp"
#include "Component/PrototypeTag.hpp"
#include "Script/PlayerMoveInputScript.hpp"
#include "Script/PreventLeavningScreen.hpp"
#include "Script/DestroyWhenLeavingScreenScript.hpp"
#include "Script/PlayerShootInputScript.hpp"

World SetupPlayground(const SDLRenderer& renderer) {
    World world;

    const auto playerBulletProtype = world.CreateEntity();
    world.EmplaceComponent(playerBulletProtype, PrototypeTag{});
    world.EmplaceComponent(playerBulletProtype, PositionComponent{ 100.0f, 100.0f });
    world.EmplaceComponent(playerBulletProtype, VelocityComponent{ 700.0f, 0.0f });
    world.EmplaceComponent(playerBulletProtype, TextureComponent{ renderer, "gfx/playerBullet.png"});
    world.EmplaceComponent(playerBulletProtype, ScriptComponent{ DestroyWhenLeavningScript{ world }});

    const auto player = world.CreateEntity();
    world.EmplaceComponent(player, KeyStateComponent{});
    world.EmplaceComponent(player, PositionComponent{ 100.0f, 100.0f });
    world.EmplaceComponent(player, VelocityComponent{ 0.0f, 0.0f });
    world.EmplaceComponent(player, TextureComponent{ renderer, "gfx/player.png" });
    world.EmplaceComponent(player, PriorityQueueComponent<ScriptComponent>())
        .Add(ScriptComponent{ PlayerMoveInputScript{ world } }, 10)
        .Add(ScriptComponent{ PreventLeavingScreen{ world }}, 1)
        .Add(ScriptComponent{ PlayerShootInputScript{ world, playerBulletProtype }}, 10);

    const EntityId playerBullet = world.CloneEntity(playerBulletProtype);
    world.RemoveComponent<PrototypeTag>(playerBullet);
    world.EmplaceComponent(playerBullet, PositionComponent{ 100.0f, 100.0f });

    return world;
}

void InvokeCallOnUpdate(World& world) {
    // Call if th player has a ScriptComponent
    for(const auto id : world.Query<ScriptComponent>()) {
        if(world.HasComponent<PrototypeTag>(id)) continue;
        world.GetComponent<ScriptComponent>(id).OnUpdate(id, SECOND_PER_UPDATE);
    }

    // Call if the player has a PriorityQueueComponent
    for(const auto id : world.Query<PriorityQueueComponent<ScriptComponent>>()) {
        if(world.HasComponent<PrototypeTag>(id)) continue;
        for(auto& scriptComponent : world.GetComponent<PriorityQueueComponent<ScriptComponent>>(id)) {
            scriptComponent.second.OnUpdate(id, SECOND_PER_UPDATE);
        }
    }
}

void InvokeCallOnEvent(World& world, const SDL_Event& event) {
    // Call if th player has a ScriptComponent
    for(const auto id : world.Query<ScriptComponent>()) {
        if(world.HasComponent<PrototypeTag>(id)) continue;
        world.GetComponent<ScriptComponent>(id).OnEvent(id, event);
    }
    // Call if the player has a PriorityQueueComponent
    for(const auto id : world.Query<PriorityQueueComponent<ScriptComponent>>()) {
        if(world.HasComponent<PrototypeTag>(id)) continue;
        for(auto& scriptComponent : world.GetComponent<PriorityQueueComponent<ScriptComponent>>(id)) {
            scriptComponent.second.OnEvent(id, event);
        }
    }
}

void InvokeMovement(World& world) {
    for(const auto id : world.Query<PositionComponent, VelocityComponent>()) {
        if(world.HasComponent<PrototypeTag>(id)) continue;

        auto& position = world.GetComponent<PositionComponent>(id);
        const auto& velocity = world.GetComponent<VelocityComponent>(id);
        position.x += velocity.dx * SECOND_PER_UPDATE;
        position.y += velocity.dy * SECOND_PER_UPDATE;
    }
}

void InvokeDrawTexture(World& world, float interpolation) {
    for(const auto id : world.Query<PositionComponent, TextureComponent>()) {
        if(world.HasComponent<PrototypeTag>(id)) continue;

        if(world.HasComponent<VelocityComponent>(id)) {
            auto& position = world.GetComponent<PositionComponent>(id);
            const auto& velocity = world.GetComponent<VelocityComponent>(id);
            const auto& texture = world.GetComponent<TextureComponent>(id);
            float x = position.x + velocity.dx * interpolation * SECOND_PER_UPDATE;
            float y = position.y + velocity.dy * interpolation * SECOND_PER_UPDATE;
            texture.Draw(x, y);
        }
        else {
            const auto& position = world.GetComponent<PositionComponent>(id);
            const auto& texture = world.GetComponent<TextureComponent>(id);
            texture.Draw(position.x, position.y);
        }
    }
}

int main() {
    try {
        SDLSystem sdlSystem;
        SDLWindow window("Shooter", SCREEN_WIDTH, SCREEN_HEIGHT);
        SDLRenderer renderer(window.get());

        bool quit = false;
        SDL_Event event;

        World world = SetupPlayground(renderer);

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
                InvokeCallOnEvent(world, event);
            }

            while(lag >= ms_per_update) {
                InvokeCallOnUpdate(world);
                InvokeMovement(world);
                lag -= ms_per_update;
            }

            SDL_RenderClear(renderer.get());
            InvokeDrawTexture(world, lag / ms_per_update);
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
std::unordered_map<std::string, std::pair<SDL_Texture*, int>> TextureComponent::textures;
