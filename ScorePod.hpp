#pragma once
#include <iostream>
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "GameEngine/Game.hpp"

constexpr float pi = 3.14156926;

class ScorePod {
    float m_x, m_y;

    struct ScorePodScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto& rect = self.GetComponent<TextureComponent>().rect;
            const auto& pos = self.GetComponent<PositionComponent>();

            if((pos.x > SCREEN_WIDTH || pos.x < -rect.w) || (pos.y > SCREEN_HEIGHT || pos.y < -rect.h)) {
                self.Destroy();
            }
        }

        void OnCollision(GameObject& self, GameObject& other) {
            self.Destroy();
            // TODO, increate score here!
        }
    };

public:
    ScorePod(float x, float y): m_x(x), m_y(y) {}

    void operator()() {
        float ran = Game::GenerateRandom(pi/2.0f, 3.0f*pi/2.0f);
        float speed = Game::GenerateRandom(ENEMY_MIN_SPEED, ENEMY_MAX_SPEED);
        float dx = cosf(ran) * speed;
        float dy = sinf(ran) * speed;
        
        GameObject()
            .AddComponent<BulletRenderLayer>()
            .AddComponent<ScorePodLayerTag>()
            .AddComponent<AABBComponent>(0.0f, 0.0f, 0.0f, 0.0f, false)
            .AddComponent<PositionComponent>(m_x, m_y)
            .AddComponent<VelocityComponent>(dx, dy)
            .AddComponent<TextureComponent>(Game::LoadTexture("gfx/points.png"))
            .AddComponent<ScriptComponent>(ScorePodScript());
    }
};
