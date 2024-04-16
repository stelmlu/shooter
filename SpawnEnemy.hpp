#include "Game.hpp"
#include "Shooter.hpp"
#include "Enemy.hpp"
#include "Component/ScriptComponent.hpp"

class SpawnEnemy {
    struct CountdownNextEnemyComponent {
        float timeout;
    };

    struct SpawnEnemyScript: public Script {
        void OnUpdate(GameObject& self, float dt) {
            auto& timeout = self.GetComponent<CountdownNextEnemyComponent>().timeout;

            if(timeout <= 0.0f) {
                AddToGame( Enemy() );
                timeout = Game::GenerateRandom(ENEMY_SPAWN_TIMEOUT_MIN, ENEMY_SPAWN_TIMEOUT_MAX);
            }

            timeout -= dt;
        }
    };

public:
    void operator()() {
        GameObject()
            .AddComponent<CountdownNextEnemyComponent>( Game::GenerateRandom(ENEMY_SPAWN_TIMEOUT_MIN, ENEMY_SPAWN_TIMEOUT_MAX) )
            .AddComponent<ScriptComponent>( SpawnEnemyScript() );
    }
};
