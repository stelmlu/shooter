#pragma once
#include "Shooter.hpp"
#include "GameEngine/GameEngine.hpp"
#include "Star.hpp"

class SpawnStar {
    struct SpawnStarScript: public Script {
        void OnConstructed(GameObject& self) {
            for(int x=0; x<SCREEN_WIDTH; x++) {
                AddToGame( Star(x) );
            }
        }

        void OnUpdate(GameObject& self, float dt) {
            AddToGame( Star(SCREEN_WIDTH) );
        };
    };

public:
    void operator()() {
        GameObject()
            .AddComponent<ScriptComponent>( SpawnStarScript() );
    }
};
