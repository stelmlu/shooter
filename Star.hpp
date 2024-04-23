#pragma once
#include <iostream>
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "GameEngine/Game.hpp"

class Star {
    float m_x;

    struct StarScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            const auto width = self.GetComponent<TextureComponent>().rect.w;
            const auto& pos = self.GetComponent<PositionComponent>(); 

            if(pos.x < -width) {
                self.Destroy();
            }
        }
    };

public:
    Star(float x): m_x(x) {}

    void operator()() {
        auto gameObject = GameObject()
            .AddComponent<StarBackgroundLayer>()
            .AddComponent<ScriptComponent>(StarScript());

        float randomNumber = Game::GenerateRandom(1.0f, 6.0f);

        if(randomNumber >= 1.0f && randomNumber <= 2.0f) {
            gameObject.AddComponent<TextureComponent>(Game::LoadTexture("gfx/star1.png"));
        }
        else if(randomNumber >= 2.0f && randomNumber <= 3.0f) {
            gameObject.AddComponent<TextureComponent>(Game::LoadTexture("gfx/star2.png"));
        }
        else if(randomNumber >= 3.0f && randomNumber <= 4.0f) {
            gameObject.AddComponent<TextureComponent>(Game::LoadTexture("gfx/star3.png"));
        }
        else if(randomNumber >= 4.0f && randomNumber <= 5.0f) {
            gameObject.AddComponent<TextureComponent>(Game::LoadTexture("gfx/star4.png"));
        }
        else {
            gameObject.AddComponent<TextureComponent>(Game::LoadTexture("gfx/star4.png"));
        }

        auto rect = gameObject.GetComponent<TextureComponent>().rect;
        gameObject.AddComponent<PositionComponent>(
            static_cast<float>(m_x + rect.w), Game::GenerateRandom(0.0f, SCREEN_HEIGHT - rect.h));

        randomNumber = Game::GenerateRandom(1.0f, 4.0f);
        uint8_t r = 255, g = 255, b = 255, a;
        if(randomNumber >= 1.0f && randomNumber <= 2.0f) {
            gameObject.AddComponent<VelocityComponent>(-300.0f);
            a = 128;
        }
        else if(randomNumber >= 2.0f && randomNumber <= 3.0f) {
            gameObject.AddComponent<VelocityComponent>(-200.0f);
            a = 92;
        }
        else {
            gameObject.AddComponent<VelocityComponent>(-100.0f);
            a = 64;
        }

        gameObject.AddComponent<AddBlenderComponent>(r, g, b, a);
    }
};
