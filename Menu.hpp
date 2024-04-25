#pragma once
#include "Shooter.hpp"
#include "GameEngine/Game.hpp"
#include "SpawnStar.hpp"
#include "Playground.hpp"

class Menu {
    struct MenuScript: public Script {
        void OnEvent(GameObject& self, const SDL_Event& event) {
            if(event.type == SDL_KEYDOWN && event.key.repeat == false && event.key.keysym.sym == SDLK_SPACE) {
                AddToGame( Playground(), true );
            }
        }
    };

public:
    void operator()() {
        auto title = GameObject()
            .AddComponent<RenderLayer4Tag>()
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/title.png") )
            .AddComponent<ScriptComponent>( MenuScript() );
        
        const auto& titleTex = title.GetComponent<TextureComponent>();

        title.AddComponent<PositionComponent>(SCREEN_WIDTH / 2.0f - titleTex.width / 2.0f, SCREEN_HEIGHT * 1.0f / 4.0f);

        auto info = GameObject()
            .AddComponent<RenderLayer4Tag>()
            .AddComponent<TextureComponent>( Game::LoadTexture("gfx/textinfo.png") );
        
        const auto& infoTex = info.GetComponent<TextureComponent>();

        info.AddComponent<PositionComponent>(SCREEN_WIDTH / 2.0f - infoTex.width/ 2.0f, SCREEN_HEIGHT * 2.0f / 4.0f);



        AddToGame( SpawnStar(), false );
    }
};
