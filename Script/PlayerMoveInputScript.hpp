#pragma once

#include "../Loom.hpp"
#include "../Component/ScriptComponent.hpp"
#include "../Component/VelocityComponent.hpp"
#include "../Component/KeyStateComponent.hpp"

class PlayerMoveInputScript: public Script {
    World& world;
    EntityId id;
public:
    PlayerMoveInputScript(World& world, EntityId id): world(world), id(id) {
        world.EmplaceComponent<KeyStateComponent>(id, {});
    }

    ~PlayerMoveInputScript() {
        world.RemoveComponent<KeyStateComponent>(id);
    }

    void OnEvent(EntityId self, const SDL_Event& event) {
        switch(event.type) {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
            case SDLK_LEFT:
                world.GetComponent<KeyStateComponent>(self).leftKeyDown = true;
                break;
            case SDLK_RIGHT:
                world.GetComponent<KeyStateComponent>(self).rightKeyDown = true;
                break;
            case SDLK_UP:
                world.GetComponent<KeyStateComponent>(self).upKeyDown = true;
                break;
            case SDLK_DOWN:
                world.GetComponent<KeyStateComponent>(self).downKeyDown = true;
                break;
            }
            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym) {
            case SDLK_LEFT:
                world.GetComponent<KeyStateComponent>(self).leftKeyDown = false;
                break;
            case SDLK_RIGHT:
                world.GetComponent<KeyStateComponent>(self).rightKeyDown = false;
                break;
            case SDLK_UP:
                world.GetComponent<KeyStateComponent>(self).upKeyDown = false;
                break;
            case SDLK_DOWN:
                world.GetComponent<KeyStateComponent>(self).downKeyDown = false;
                break;
            }
        }
    }

    void OnUpdate(EntityId self, float dt) {
        const auto& keyState = world.GetComponent<KeyStateComponent>(self);
        auto& velocity = world.EmplaceComponent<VelocityComponent>(self, { 0.0f, 0.0f });

        if(keyState.leftKeyDown) {
            velocity.dx = -PLAYER_MAX_SPEED;
        }
        else if(keyState.rightKeyDown) {
            velocity.dx = PLAYER_MAX_SPEED;
        }
        
        if(keyState.upKeyDown) {
            velocity.dy = -PLAYER_MAX_SPEED;
        }
        else if(keyState.downKeyDown) {
            velocity.dy = PLAYER_MAX_SPEED;
        }
    }
};