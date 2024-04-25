#pragma once
#include "Shooter.hpp"
#include "GameEngine/Game.hpp"
#include "Explosion.hpp"
#include "GameOverTimeoutComponent.hpp"

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
                AddToGame(Explosion(position.x + texture.width / 2.0f, position.y + texture.height / 2.0f));
            }

            Game::FindGameObject("playground")
                .AddComponent<GameOverTimeoutComponent>( true, 1.0f ); // 1s

            self.Destroy();
            other.Destroy();
        }
    };

public:
    void operator()() {
        auto player = GameObject()
            .AddComponent<SpaceShipRenderLayer>()
            .AddComponent<SearchableComponent>("player")
            .AddComponent<VelocityComponent>(0.0f, 0.0f)
            .AddComponent<TextureComponent>(Game::LoadTexture("gfx/player.png"))
            .AddComponent<KeyStateComponent>()
            .AddComponent<FireCooldown>()
            .AddComponent<ScriptComponent>(PlayerScript{})
            .AddComponent<EnemyColitionLayerTag>()
            .AddComponent<EnemyBulletColitionLayerTag>()
            .AddComponent<ScorePodLayerTag>()
            .AddComponent<AABBComponent>(20.0f, 0.0f, -20.0f, 0.0f, false);
        
        const auto& tex = player.GetComponent<TextureComponent>();

        player.AddComponent<PositionComponent>(SCREEN_WIDTH / 2.0f - tex.width / 2.0f, SCREEN_HEIGHT / 2.0f - tex.height / 2.0f );
    };
};
