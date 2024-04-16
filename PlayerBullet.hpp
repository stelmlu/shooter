#include "Game.hpp"
#include "Settings.hpp"
#include "Component/PositionComponent.hpp"
#include "Component/VelocityComponent.hpp"
#include "Component/ScriptComponent.hpp"

class PlayerBullet {
    float m_x, m_y;

    struct PlayerBulletScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto& position = self.GetComponent<PositionComponent>();
            const auto& texture = self.GetComponent<TextureComponent>();

            if((position.x + texture.width) > SCREEN_WIDTH) {
                self.Destroy();
            }
        }
    };

public:
    PlayerBullet(float x, float y): m_x(x), m_y(y) {}

    void operator()() {
        auto gameObject = GameObject()
            .AddComponent<VelocityComponent>( PLAYER_BULLET_SPEED, 0.0f )
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/playerBullet.png") )
            .AddComponent<ScriptComponent>(PlayerBulletScript());
        
        auto& textureComponent = gameObject.GetComponent<TextureComponent>();

        gameObject.AddComponent<PositionComponent>(
            m_x + static_cast<float>(textureComponent.width),
            m_y - static_cast<float>(textureComponent.height) / 2.0f);
    }
};