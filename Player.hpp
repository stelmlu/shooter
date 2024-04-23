#include "Shooter.hpp"
#include "GameEngine/Game.hpp"
#include "Explosion.hpp"

class Player {
    struct KeyStateComponent {
        bool leftKeyDown = false;
        bool rightKeyDown = false;
        bool upKeyDown = false;
        bool downKeyDown = false;
        bool fireKeyDown = false;
    };

    struct FireCooldown {
        float timeout;
    };

    struct PlayerScript: public Script {
        void OnEvent(GameObject& self, const SDL_Event& event);
        void OnUpdate(GameObject& self, float dt);
        void OnCollision(GameObject& self, GameObject& other) {
            const auto& position = other.GetComponent<PositionComponent>();
            const auto& texture = other.GetComponent<TextureComponent>();
            for(int i=0; i<15; i++) {
                AddToGame(Explosion(position.x + texture.rect.w / 2.0f, position.y + texture.rect.h / 2.0f));
            }

            self.Destroy();
            other.Destroy();
        }
    };

public:
    void operator()() {
        GameObject()
            .AddComponent<SpaceShipRenderLayer>()
            .AddComponent<SearchableComponent>("player")
            .AddComponent<PositionComponent>(Game::GenerateRandom(100.0f, 400.0f), 100.f)
            .AddComponent<VelocityComponent>(0.0f, 0.0f)
            .AddComponent<TextureComponent>(Game::LoadTexture("gfx/player.png"))
            .AddComponent<KeyStateComponent>()
            .AddComponent<FireCooldown>()
            .AddComponent<ScriptComponent>(PlayerScript{})
            .AddComponent<EnemyColitionLayerTag>()
            .AddComponent<EnemyBulletColitionLayerTag>()
            .AddComponent<ScorePodLayerTag>()
            .AddComponent<AABBComponent>(20.0f, 0.0f, -20.0f, 0.0f, false);
    };
};
