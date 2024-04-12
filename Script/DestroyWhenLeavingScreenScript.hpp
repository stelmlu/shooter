#pragma once

#include <entity/registry.hpp>
#include "../Settings.hpp"
#include "../Component/ScriptComponent.hpp"
#include "../Component/VelocityComponent.hpp"
#include "../Component/TextureComponent.hpp"

struct DestroyWhenLeavningScreenScript: public Script {
    void OnUpdate(entt:: registry& reg, entt::entity self, float dt) {
        const auto& position = reg.get<PositionComponent>(self);
        auto& velocity  = reg.get<VelocityComponent>(self);
        const auto& textureEntity = reg.get<TextureEntityComponent>(self);
        const auto& texture = reg.get<TextureComponent>(textureEntity.entity);

        float x = position.x;
        float y = position.y;
        float w = texture.width;
        float h = texture.height;
        float nextX = x + velocity.dx * dt;
        float nextY = y + velocity.dy * dt;

        if ((nextX + w) < 0 || nextX > SCREEN_WIDTH) {
            reg.destroy(self);
        }

        if ((nextY + w) < 0 || nextY > SCREEN_HEIGHT) {
            reg.destroy(self);
        }
    }
};
