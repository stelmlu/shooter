#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "Loom.hpp"
#include "SDLSystem.hpp"
#include "SDLWindow.hpp"
#include "SDLRenderer.hpp"
#include "Component/PositionComponent.hpp"
#include "Component/TextureComponent.hpp"
#include "Component/ScriptComponent.hpp"
#include "Component/PriorityQueueComponent.hpp"

// Screen dimension constants
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

World SetupPlayground(const SDLRenderer& renderer) {
    World world;

    const auto player = world.CreateEntity();
    world.EmplaceComponent(player, PositionComponent{ 100.0f, 100.0f });
    world.EmplaceComponent(player, TextureComponent(renderer, "gfx/player.png"));

    return world;
}

void InvokeCallOnUpdate(World& world, float dt) {
    // Call if th player has a ScriptComponent
    for(const auto id : world.Query<ScriptComponent>()) {
        world.GetComponent<ScriptComponent>(id).OnUpdate(id, dt);
    }
    // Call if the player has a PriorityQueueComponent
    for(const auto id : world.Query<PriorityQueueComponent<ScriptComponent>>()) {
        for(auto& scriptComponent : world.GetComponent<PriorityQueueComponent<ScriptComponent>>(id)) {
            scriptComponent.second.OnUpdate(id, dt);
        }
    }
}

void InvokeDrawTexture(World& world) {
    for(const auto id : world.Query<PositionComponent, TextureComponent>()) {
        const auto& position = world.GetComponent<PositionComponent>(id);
        const auto& texture = world.GetComponent<TextureComponent>(id);
        texture.Draw(position.x, position.y);
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

        while (!quit) {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
            }

            InvokeCallOnUpdate(world, 1.0f / 60.0f);

            SDL_RenderClear(renderer.get());
            InvokeDrawTexture(world);
            SDL_RenderPresent(renderer.get());
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}

// Static member definition
std::unordered_map<std::string, std::pair<SDL_Texture*, int>> TextureComponent::textures;
