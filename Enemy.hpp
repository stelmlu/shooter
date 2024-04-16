#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "EnemyBullet.hpp"

class Enemy {
    struct EnemyScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto& position = self.GetComponent<PositionComponent>();
            const auto& texture = self.GetComponent<TextureComponent>();

            if((position.x + texture.width) < 0.0f) {
                self.Destroy();
            }
        }
    };

public:
    void operator()() {
        auto& self = GameObject()
            .AddComponent<VelocityComponent>( -Game::GenerateRandom(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED), 0.0f)
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/enemy.png") )
            .AddComponent<ScriptComponent>( EnemyScript() )
            .AddComponent<PlayerBulletTag>()
            .AddComponent<EnemyTag>();
        
        const auto& texture = self.GetComponent<TextureComponent>();
        float x = SCREEN_WIDTH + texture.width;
        float y = Game::GenerateRandom(0.0f, SCREEN_HEIGHT - texture.height);
        self.AddComponent<PositionComponent>(x, y);
    }
};
