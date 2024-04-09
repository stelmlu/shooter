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

// Screen dimension constants
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

struct BaseScript {

    void OnSetup(EntityId self) {
        std::cout << "Base OnSetup: " << self << "\n";
    }

    void OnEvent(EntityId self, const SDL_Event& event) {}

    void OnUpdate(EntityId self, float dt) {}

    void OnCollision(EntityId self, EntityId other) {}

    void OnDestroyed(EntityId self) {
        std::cout << "OnDestroy: " << self << "\n";
    }
};

struct MyScript: BaseScript {
    void OnSetup(EntityId self) {
        std::cout << "My OnSetup: " << self << "\n";
    }
};

World SetupGamePlay(const SDLRenderer& renderer) {
    World world;

    const auto player = world.CreateEntity();
    world.EmplaceComponent(player, PositionComponent{ 100.0f, 100.0f });
    world.EmplaceComponent(player, TextureComponent(renderer, "gfx/player.png"));
    world.EmplaceComponent(player, ScriptComponent( MyScript{} ));

    const auto foo = world.CloneEntity(player);

    return world;
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

        World world = SetupGamePlay(renderer);

        while (!quit) {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
            }

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
