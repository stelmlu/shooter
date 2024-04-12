#pragma once

#include <entity/registry.hpp>
#include <functional>
#include "PlayerMoveInputScript.hpp"
#include "PreventLeavningScreenScript.hpp"
#include "PlayerShootInputScript.hpp"

class PlayerProxyScript: public Script {
    entt::registry& reg;
    PlayerMoveInputScript moveInputScript;
    PreventLeavingScreenScript preventLeavingScreenScript;
    PlayerShootInputScript playerShootInputScript;
public:
    template<typename Func>
    PlayerProxyScript(entt::registry& reg, const SDLRenderer& renderer, Func& createPlayerBullet)
        : reg(reg)
        , moveInputScript(reg)
        , preventLeavingScreenScript(reg)
        , playerShootInputScript(reg, renderer, createPlayerBullet) {}
    
    void OnConstructed(entt::entity self) {
        playerShootInputScript.OnConstructed(self);
    }
    
    void OnEvent(entt::entity self, const SDL_Event& event) {
        moveInputScript.OnEvent(self, event);
        playerShootInputScript.OnEvent(self, event);
    }

    void OnUpdate(entt::entity self, float dt) {
        moveInputScript.OnUpdate(self, dt);
        preventLeavingScreenScript.OnUpdate(self, dt);
        playerShootInputScript.OnUpdate(self, dt);
    }

    void OnDestructed(entt::entity self) {
        playerShootInputScript.OnDestroyed(self);
    }
};
