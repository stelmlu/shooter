#pragma once

#include <SDL2/SDL.h>
#include <entity/registry.hpp>
#include <functional>
#include "../Settings.hpp"
#include "../Component/PositionComponent.hpp"
#include "../Component/KeyStateComponent.hpp"
#include "../Component/FireCooldownComponent.hpp"

class PlayerShootInputScript: public Script {
    std::function<entt::entity(entt::registry&)> createPlayerBullet;

public:
    template<typename Func>
    PlayerShootInputScript(Func& createPlayerBullet)
        : createPlayerBullet(createPlayerBullet) {}
    
    void OnConstructed(entt:: registry& reg, entt::entity self) {
        reg.emplace<FireCooldownComponent>(self, PLAYER_FIRE_COOLDOWN_TIME);
    }

    void OnEvent(entt:: registry& reg, entt::entity self, const SDL_Event& event) {
        if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
            reg.get<KeyStateComponent>(self).fireKeyDown = true;
        }
        else if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE) {
            reg.get<KeyStateComponent>(self).fireKeyDown = false;
        }
    }

    void OnUpdate(entt:: registry& reg, entt::entity self, float dt) {
        auto& fireCooldown = reg.get<FireCooldownComponent>(self);
        auto fireDown = reg.get<KeyStateComponent>(self).fireKeyDown;
        fireCooldown.countdown -= dt;

        if(fireDown && fireCooldown.countdown < 0.0f) {
            const auto& position = reg.get<PositionComponent>(self);

            const entt::entity playerBullet = createPlayerBullet(reg);
            reg.get<FireCooldownComponent>(self).countdown = PLAYER_FIRE_COOLDOWN_TIME;
            reg.replace<PositionComponent>(playerBullet, position.x + 48, position.y + 16);
        }
    }

    void OnDestroyed(entt:: registry& reg, entt::entity self) {
        reg.remove<FireCooldownComponent>(self);
    }
};
