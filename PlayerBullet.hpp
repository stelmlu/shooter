#include "Game.hpp"
#include "Settings.hpp"
#include "Component/PositionComponent.hpp"
#include "Component/VelocityComponent.hpp"
#include "Component/ScriptComponent.hpp"

class PlayerBullet {
    float m_x, m_y;

    struct PlayerBulletScript: public Script {
        void OnUpdate(entt:: registry& reg, entt::entity self, float dt) {
            const auto& position = reg.get<PositionComponent>(self);
            const auto& texture = reg.get<TextureComponent>(self);

            if((position.x + texture.width) > SCREEN_WIDTH) {
                reg.destroy(self);
                std::cout << "Bullet destroyed!\n";
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
