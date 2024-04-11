#include "../Loom.hpp"
#include "../Settings.hpp"
#include "../Component/PositionComponent.hpp"
#include "../Component/KeyStateComponent.hpp"
#include "../Component/PrototypeTag.hpp"

class PlayerShootInputScript: public Script {
    World& world;
    EntityId playerBulletProtype;
public:
    PlayerShootInputScript(World& world, EntityId playerBulletProtype)
        : world(world)
        , playerBulletProtype(playerBulletProtype) {}

    void OnEvent(EntityId self, const SDL_Event& event) {
        if(event.type == SDL_KEYDOWN) {
            if(event.key.repeat != 0) return;

            if(event.key.keysym.sym == SDLK_SPACE) {
                const auto& position = world.GetComponent<PositionComponent>(self);

                const EntityId playerBullet = world.CloneEntity(playerBulletProtype);
//                 world.RemoveComponent<PrototypeTag>(playerBullet);
//                 world.EmplaceComponent(playerBullet, PositionComponent{ position.x + 10, position.y + 16 });
            }
        }
    }
};
