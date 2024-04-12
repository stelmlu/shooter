#pragma once

#include <entity/registry.hpp>
#include <functional>
#include "PlayerMoveInputScript.hpp"
#include "PreventLeavningScreenScript.hpp"
#include "PlayerShootInputScript.hpp"

class PlayerProxyScript: public Script {
    PlayerMoveInputScript moveInputScript;
    PreventLeavingScreenScript preventLeavingScreenScript;
    PlayerShootInputScript playerShootInputScript;
public:
    template<typename Func>
    PlayerProxyScript(Func& createPlayerBullet)
        : moveInputScript()
        , preventLeavingScreenScript()
        , playerShootInputScript(createPlayerBullet) {}
    
    void OnConstructed(entt:: registry& reg, entt::entity self) {
        playerShootInputScript.OnConstructed(reg, self);
    }
    
    void OnEvent(entt:: registry& reg, entt::entity self, const SDL_Event& event) {
        moveInputScript.OnEvent(reg, self, event);
        playerShootInputScript.OnEvent(reg, self, event);
    }

    void OnUpdate(entt:: registry& reg, entt::entity self, float dt) {
        moveInputScript.OnUpdate(reg, self, dt);
        preventLeavingScreenScript.OnUpdate(reg, self, dt);
        playerShootInputScript.OnUpdate(reg, self, dt);
    }

    void OnDestructed(entt:: registry& reg, entt::entity self) {
        playerShootInputScript.OnDestroyed(reg, self);
    }
};
