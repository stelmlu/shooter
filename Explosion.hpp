#pragma once
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "GameEngine/Game.hpp"

class Explosion {
    float m_x, m_y;

    struct TimeoutToDestroy {
        float timeout;
    };

    struct ExplisionScript: Script {
        void OnUpdate(GameObject& self, float dt) {
            auto& timeout = self.GetComponent<TimeoutToDestroy>().timeout;
            auto& alpha = self.GetComponent<AddBlenderComponent>().a;

            if(timeout <= 0.0f) {
                self.Destroy();
            }

            timeout -= dt;

            // Decrease the alpha value to fade it away.
            alpha -= 255 * 0.7 * dt;
        }
    };

public:
    Explosion(float x, float y): m_x(x), m_y(y) { }

    void operator()() {
        auto explosion = GameObject()
            .AddComponent<BulletRenderLayer>()
            .AddComponent<PositionComponent>(m_x + Game::GenerateRandom(-32.0f, 32.0f), m_y + Game::GenerateRandom(-32.0f, 32.0f))
            .AddComponent<VelocityComponent>(Game::GenerateRandom(-100.0f, 100.0f), Game::GenerateRandom(-100.0f, 100.0f))
            .AddComponent<TextureComponent>(Game::LoadTexture("gfx/explosion.png"))
            .AddComponent<TimeoutToDestroy>(0.7f)
            .AddComponent<ScriptComponent>(ExplisionScript());
            
            // Randomize a color for the explostion
            auto randomNumber  = Game::GenerateRandom(1.0f, 5.0f);
            uint8_t r,g,b,a;
            if(randomNumber > 1.0f && randomNumber <= 2.0f) {
                r = 255;
                g = 0;
                b = 0;
            }
            else if(randomNumber > 2.0f && randomNumber <= 3.0f) {
                r = 255;
                g = 128;
                b = 0;

            }
            else if(randomNumber > 3.0f && randomNumber <= 4.0f) {
                r = 255;
                g = 255;
                b = 0;

            }
            else {
                r = 255;
                g = 255;
                b = 255;
            }
            a = static_cast<uint8_t>(Game::GenerateRandom(128.0f, 255.0f));
            explosion.AddComponent<AddBlenderComponent>(r, g, b, a);
    }
};
