#pragma once
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "Player.hpp"
#include "SpawnEnemy.hpp"
#include "SpawnStar.hpp"
#include "ScoreLabel.hpp"
#include "Menu.hpp"
#include "GameOverTimeoutComponent.hpp"
#include "GameOver.hpp"

class Playground {
    struct PlaygroundScript: Script {
        void OnUpdate(GameObject& self, float dt) {
            if(self.HasComponent<GameOverTimeoutComponent>()) {
                auto& gameOverTimeout = self.GetComponent<GameOverTimeoutComponent>();

                 if(gameOverTimeout.isActive && gameOverTimeout.timeout <= 0.0f) {
                    AddToGame( GameOver() );
                    self.Destroy();
                }

                gameOverTimeout.timeout -= dt;
            }
        }
    };

public:
    void operator()() {
        GameObject()
            .AddComponent<SearchableComponent>("playground")
            .AddComponent<ScriptComponent>( PlaygroundScript() );

        AddToGame( Player() );
        AddToGame( SpawnEnemy() );
        AddToGame( SpawnStar() );
        AddToGame( ScoreLabel() );
    }
};
