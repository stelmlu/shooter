#include "Shooter.hpp"
#include "GameEngine/Game.hpp"

class PlayerBullet {
    float m_x, m_y;

    struct PlayerBulletScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto& position = self.GetComponent<PositionComponent>();
            const auto& texture = self.GetComponent<TextureComponent>();

            if((position.x + texture.rect.w) > SCREEN_WIDTH) {
                self.Destroy();
            }
        }

        void OnCollision(GameObject& self, GameObject& other) {
            self.Destroy();
            other.Destroy();
        }
    };

public:
    PlayerBullet(float x, float y): m_x(x), m_y(y) {}

    void operator()() {
        auto gameObject = GameObject()
            .AddComponent<VelocityComponent>( PLAYER_BULLET_SPEED, 0.0f )
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/playerbullet.png") )
            .AddComponent<ScriptComponent>(PlayerBulletScript())
            .AddComponent<PlayerBulletTag>();
        
        auto& textureComponent = gameObject.GetComponent<TextureComponent>();

        gameObject.AddComponent<PositionComponent>(
            m_x + static_cast<float>(textureComponent.rect.w),
            m_y - static_cast<float>(textureComponent.rect.h) / 2.0f);
    }
};
