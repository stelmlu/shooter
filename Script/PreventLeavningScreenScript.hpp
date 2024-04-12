#pragma once
#include <entity/registry.hpp>
#include "../Settings.hpp"
#include "../Component/ScriptComponent.hpp"
#include "../Component/VelocityComponent.hpp"
#include "../Component/TextureComponent.hpp"

struct PreventLeavingScreenScript: public Script {
    void OnUpdate(entt:: registry& reg, entt::entity self, float dt) {
        const auto& position = reg.get<PositionComponent>(self);
        auto& velocity  = reg.get<VelocityComponent>(self);
        const auto& texture = reg.get<TextureComponent>(self);

        float x = position.x;
        float y = position.y;
        float w = texture.width;
        float h = texture.height;
        float nextX = x + velocity.dx * dt;
        float nextY = y + velocity.dy * dt;

        if (nextX < 0 || (nextX + w) > SCREEN_WIDTH) {
            velocity.dx = 0;
        }

        if (nextY < 0 || (nextY + h) > SCREEN_HEIGHT) {
            velocity.dy = 0;
        }
    }
};
