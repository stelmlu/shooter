#include "Shooter.hpp"
#include "GameEngine/Game.hpp"
#include "GameEngine/GameEngine.hpp"
#include "Explosion.hpp"
#include "ScorePod.hpp"

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

        void OnCollision(GameObject& self, GameObject& other) {
            const auto& position = other.GetComponent<PositionComponent>();
            const auto& texture = other.GetComponent<TextureComponent>();
            for(int i=0; i<10; i++) {
                AddToGame(Explosion(position.x + texture.width / 2.0f, position.y + texture.height / 2.0f));
            }

            AddToGame( ScorePod(position.x + texture.width / 2.0f, position.y + texture.height / 2.0f) );

            self.Destroy();
            other.Destroy();
        }
    };

public:
    PlayerBullet(float x, float y): m_x(x), m_y(y) {}

    void operator()() {
        auto gameObject = GameObject()
            .AddComponent<BulletRenderLayer>()
            .AddComponent<VelocityComponent>( PLAYER_BULLET_SPEED, 0.0f )
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/playerbullet.png") )
            .AddComponent<ScriptComponent>(PlayerBulletScript())
            .AddComponent<PlayerBulletColitionLayerTag>()
            .AddComponent<AABBComponent>(0.0f, 0.0f, 0.0f, 0.0f, false);
        
        auto& textureComponent = gameObject.GetComponent<TextureComponent>();

        gameObject.AddComponent<PositionComponent>(
            m_x + static_cast<float>(textureComponent.width),
            m_y - static_cast<float>(textureComponent.height) / 2.0f);
    }
};
