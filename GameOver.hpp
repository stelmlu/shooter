#pragma once
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "GameEngine/Game.hpp"

class GameOver {
    struct GameOverScript: public Script {
        void OnEvent(GameObject& self, const SDL_Event& event);
    };

public:
    void operator()() {
        auto gameOver = GameObject()
            .AddComponent<RenderLayer6Tag>()
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/textgameover.png"))
            .AddComponent<ScriptComponent>( GameOverScript() );
        
        auto gameOverTex = gameOver.GetComponent<TextureComponent>();

        gameOver.AddComponent<PositionComponent>(
            SCREEN_WIDTH / 2.0f - gameOverTex.width / 2.0f, SCREEN_HEIGHT / 2.0f - gameOverTex.height / 2.0f);
    }
};
