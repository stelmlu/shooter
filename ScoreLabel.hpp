#pragma once
#include "GameEngine/GameEngine.hpp"

class ScoreLabel {
public:
    struct ScoreLabelComponent {
        int score;
        GameObject scoreText;
        GameObject digit0;
        GameObject digit1;
        GameObject digit2;
    };

private:
    struct ScoreLabelScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            auto score = self.GetComponent<ScoreLabelComponent>();

            // Update the texture depended on the score
            if(score.score >= 0 && score.score <= 9) {
                std::string dig0 = std::string("gfx/num") + std::to_string(score.score) + ".png";
                score.digit0.AddComponent<TextureComponent>( Game::LoadTexture(dig0.c_str()) );
                score.digit1.RemoveComponent<TextureComponent>();
                score.digit2.RemoveComponent<TextureComponent>();
            }
            else if(score.score >= 10 && score.score <= 99) {
                std::string dig0 = std::string("gfx/num") + std::to_string((score.score / 10) % 10) + ".png";
                std::string dig1 = std::string("gfx/num") + std::to_string(score.score % 10) + ".png";
                score.digit0.AddComponent<TextureComponent>( Game::LoadTexture(dig0.c_str()) );
                score.digit1.AddComponent<TextureComponent>( Game::LoadTexture(dig1.c_str()) );
                score.digit2.RemoveComponent<TextureComponent>();
            }
            else if(score.score >= 100 && score.score <= 999) {
                std::string dig0 = std::string("gfx/num") + std::to_string((score.score / 100) % 10) + ".png";
                std::string dig1 = std::string("gfx/num") + std::to_string((score.score / 10) % 10) + ".png";
                std::string dig2 = std::string("gfx/num") + std::to_string(score.score % 10) + ".png";
                score.digit0.AddComponent<TextureComponent>( Game::LoadTexture(dig0.c_str()) );
                score.digit1.AddComponent<TextureComponent>( Game::LoadTexture(dig1.c_str()) );
                score.digit2.AddComponent<TextureComponent>( Game::LoadTexture(dig2.c_str()) );
            }
        }
    };

public:
    void operator()() {
        GameObject()
            .AddComponent<SearchableComponent>("score")
            .AddComponent<ScoreLabelComponent>(0
                , GameObject()
                    .AddComponent<TextRenderLayer>()
                    .AddComponent<PositionComponent>(SCREEN_WIDTH - 256.0f, 30.0f)
                    .AddComponent<TextureComponent>(Game::LoadTexture("gfx/textscore.png"))
                , GameObject()
                    .AddComponent<TextRenderLayer>()
                    .AddComponent<PositionComponent>(SCREEN_WIDTH - 156.0f, 30.0f)
                    .AddComponent<TextureComponent>(Game::LoadTexture("gfx/num0.png"))
                , GameObject()
                    .AddComponent<TextRenderLayer>()
                    .AddComponent<PositionComponent>(SCREEN_WIDTH - 134.0f, 30.0f)
                , GameObject()
                    .AddComponent<TextRenderLayer>()
                    .AddComponent<PositionComponent>(SCREEN_WIDTH - 112.0f, 30.0f))
            .AddComponent<ScriptComponent>( ScoreLabelScript() );
    }
};
