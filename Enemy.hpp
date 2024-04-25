#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "EnemyBullet.hpp"
#include "Explosion.hpp"

class Enemy {
    struct SpawnEnemyBulletTimeoutComponent {
        float timeout;
    };

    struct EnemyScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto& position = self.GetComponent<PositionComponent>();
            const auto& texture = self.GetComponent<TextureComponent>();
            auto& spawnTimeout = self.GetComponent<SpawnEnemyBulletTimeoutComponent>().timeout;

            // Destroy the enemy if it leaves the screen
            if((position.x + texture.width) < 0.0f) {
                self.Destroy();
            }

            // Shoot in direction of the player
            if(spawnTimeout <= 0.0f) {
                auto player = Game::FindGameObject("player");

                if(player.IsValid()) {
                    const auto& playerPos = player.GetComponent<PositionComponent>();
                    const auto& playerTex = player.GetComponent<TextureComponent>();
                    AddToGame(EnemyBullet(
                        position.x + texture.width / 2.0f,
                        position.y + texture.height / 2.0f,
                        playerPos.x + playerTex.width / 2.0f,
                        playerPos.y + playerTex.height / 2.0f));
                }

                spawnTimeout = Game::GenerateRandom(ENEMY_BULLET_SPAWN_TIMEOUT_MIN, ENEMY_BULLET_SPAWN_TIMEOUT_MAX);
            }

            spawnTimeout -= dt;
        }
    };

public:
    void operator()() {
        auto& self = GameObject()
            .AddComponent<SpaceShipRenderLayer>()
            .AddComponent<VelocityComponent>( -Game::GenerateRandom(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED), 0.0f)
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/enemy.png") )
            .AddComponent<ScriptComponent>( EnemyScript() )
            .AddComponent<SpawnEnemyBulletTimeoutComponent>(Game::GenerateRandom(ENEMY_BULLET_SPAWN_TIMEOUT_MIN, ENEMY_BULLET_SPAWN_TIMEOUT_MAX))
            .AddComponent<PlayerBulletColitionLayerTag>()
            .AddComponent<EnemyColitionLayerTag>()
            .AddComponent<AABBComponent>(20.0f, 0.0f, -20.0f, 0.0f, false);
        
        const auto& texture = self.GetComponent<TextureComponent>();
        float x = SCREEN_WIDTH + texture.width;
        float y = Game::GenerateRandom(0.0f, SCREEN_HEIGHT - texture.width);
        self.AddComponent<PositionComponent>(x, y);
    }
};
