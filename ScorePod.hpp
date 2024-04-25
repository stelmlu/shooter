#pragma once
#include <iostream>
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "GameEngine/Game.hpp"
#include "ScoreLabel.hpp"

constexpr float pi = 3.14156926;

class ScorePod {
    float m_x, m_y;

    struct ScorePodScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto& tex = self.GetComponent<TextureComponent>();
            const auto& pos = self.GetComponent<PositionComponent>();

            if((pos.x > SCREEN_WIDTH || pos.x < -tex.width) || (pos.y > SCREEN_HEIGHT || pos.y < -tex.height)) {
                self.Destroy();
            }
        }

        void OnCollision(GameObject& self, GameObject& other) {
            self.Destroy();

            // Update the score with one
            Game::FindGameObject("score").GetComponent<ScoreLabel::ScoreLabelComponent>().score++;
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
