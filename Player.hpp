#include "Shooter.hpp"
#include "Game.hpp"

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
            self.Destroy();
            other.Destroy();
        }
    };

public:
    void operator()() {
        GameObject()
            .AddComponent<PositionComponent>(Game::GenerateRandom(100.0f, 400.0f), 100.f)
            .AddComponent<VelocityComponent>(0.0f, 0.0f)
            .AddComponent<TextureComponent>(Game::LoadTexture("gfx/player.png"))
            .AddComponent<KeyStateComponent>()
            .AddComponent<FireCooldown>()
            .AddComponent<ScriptComponent>(PlayerScript{})
            .AddComponent<EnemyTag>();
    };
};
