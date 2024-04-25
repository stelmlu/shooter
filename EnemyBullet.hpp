#pragma once
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"

class EnemyBullet {
    float m_enemyX, m_enemyY;
    float m_playerX, m_playerY;

    struct EnemyBulletScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto& pos = self.GetComponent<PositionComponent>();
            const auto& tex = self.GetComponent<TextureComponent>();

            if((pos.x + tex.width) < 0.0f) {
                self.Destroy();
            }

            if((pos.y + tex.height) < 0.0f) {
                self.Destroy();
            }

            if(pos.x > SCREEN_WIDTH) {
                self.Destroy();
            }

            if(pos.y > SCREEN_HEIGHT) {
                self.Destroy();
            }
        }
    };

public:
    EnemyBullet(float enemyX, float enemyY, float playerX, float playerY)
        : m_enemyX(enemyX)
        , m_enemyY(enemyY)
        , m_playerX(playerX)
        , m_playerY(playerY)
    { }

    void operator()() {
        // Calculate the direction vector
        float dx = m_playerX - m_enemyX;
        float dy = m_playerY - m_enemyY;
        float m = std::sqrt(dx * dx + dy * dy);
        if(m > 0.0f) {
            dx = dx / m;
            dy = dy / m;
        }
        else {
            dx = dy = 0.0f;
        }

        auto gameObject = GameObject()
            .AddComponent<BulletRenderLayer>()
            .AddComponent<VelocityComponent>(dx * ENEMY_BULLET_SPEED, dy * ENEMY_BULLET_SPEED)
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/enemybullet.png"))
            .AddComponent<ScriptComponent>( EnemyBulletScript() )
            .AddComponent<EnemyBulletColitionLayerTag>()
            .AddComponent<AABBComponent>(0.0f, 0.0f, 0.0f, 0.0f, false);
        
        const auto& tex = gameObject.GetComponent<TextureComponent>();

        gameObject.AddComponent<PositionComponent>(
            m_enemyX + tex.width / 2.0f,
            m_enemyY + tex.height / 2.0f);
    }
};