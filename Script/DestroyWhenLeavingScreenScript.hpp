#include "../Loom.hpp"
#include "../Settings.hpp"
#include "../Component/ScriptComponent.hpp"
#include "../Component/VelocityComponent.hpp"
#include "../Component/TextureComponent.hpp"

class DestroyWhenLeavningScript: public Script {
    World& world;
public:
    DestroyWhenLeavningScript(World& world): world(world) {}

    void OnUpdate(EntityId self, float dt) {
        const auto& position = world.GetComponent<PositionComponent>(self);
        auto& velocity  = world.GetComponent<VelocityComponent>(self);
        const auto& texture = world.GetComponent<TextureComponent>(self);

        float x = position.x;
        float y = position.y;
        auto textureSize = texture.GetSize();
        float w = textureSize.first;
        float h = textureSize.second;
        float nextX = x + velocity.dx * dt;
        float nextY = y + velocity.dy * dt;

        if ((nextX + w) < 0 || nextX > SCREEN_WIDTH) {
            world.DestroyEntity(self);
        }

        if ((nextY + w) < 0 || nextY > SCREEN_HEIGHT) {
            world.DestroyEntity(self);
        }
    }
};
