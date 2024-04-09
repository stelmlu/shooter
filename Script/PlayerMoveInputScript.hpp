#pragma once

#include "../Loom.hpp"
#include "../Settings.hpp"
#include "../Component/ScriptComponent.hpp"
#include "../Component/VelocityComponent.hpp"
#include "../Component/KeyStateComponent.hpp"

class PlayerMoveInputScript: public Script {
    World& world;
public:
    PlayerMoveInputScript(World& world, EntityId self): world(world) {
        world.EmplaceComponent<KeyStateComponent>(self, {});
        world.EmplaceComponent<VelocityComponent>(self, { 0.0f, 0.0f });
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
        auto& velocity = world.GetComponent<VelocityComponent>(self);

        // Apply acceleration if key is pressed.
        if(keyState.leftKeyDown) {
            velocity.dx -= PLAYER_ACCELERATION * dt;
            velocity.dx = std::max(velocity.dx, -PLAYER_MAX_SPEED);
        }
        if(keyState.rightKeyDown) {
            velocity.dx += PLAYER_ACCELERATION * dt;
            velocity.dx = std::min(velocity.dx, PLAYER_MAX_SPEED);
        }
        if(keyState.upKeyDown) {
            velocity.dy -= PLAYER_ACCELERATION * dt;
            velocity.dy = std::max(velocity.dy, -PLAYER_MAX_SPEED);
        }
        if(keyState.downKeyDown) {
            velocity.dy += PLAYER_ACCELERATION * dt;
            velocity.dy = std::min(velocity.dy, PLAYER_MAX_SPEED);
        }

        if(!keyState.leftKeyDown && !keyState.rightKeyDown) {
            if(velocity.dx < 0.0f) {
                velocity.dx += PLAYER_DEACCELERATION * dt;
                velocity.dx = std::min(velocity.dx, 0.0f);
            }
            else if(velocity.dx > 0.0f) {
                velocity.dx -= PLAYER_DEACCELERATION * dt;
                velocity.dx = std::max(velocity.dx, 0.0f);
            }
        }

        if(!keyState.upKeyDown && !keyState.downKeyDown) {
            if(velocity.dy < 0.0f) {
                velocity.dy += PLAYER_DEACCELERATION * dt;
                velocity.dy = std::min(velocity.dy, 0.0f);
            }
            else if(velocity.dy > 0.0f) {
                velocity.dy -= PLAYER_DEACCELERATION * dt;
                velocity.dy = std::max(velocity.dy, 0.0f);
            }
        }
    }
};