#pragma once
#include <SDL.h>
#include <cstdint>
#include <memory>
#include <entity/registry.hpp>

struct Script {
    void OnConstructed(entt::entity self) {}
    void OnEvent(entt::entity self, const SDL_Event& event) {}
    void OnUpdate(entt::entity self, float dt) {}
    void OnCollision(entt::entity self, entt::entity other) {}
    void OnDestroyed(entt::entity self) {}
};

class ScriptComponent {
public:
    struct IScript {
        virtual ~IScript() = default;
        virtual void OnConstructed(entt::entity self) = 0;
        virtual void OnEvent(entt::entity self, const SDL_Event& event) = 0;
        virtual void OnUpdate(entt::entity self, float dt) = 0;
        virtual void OnCollision(entt::entity self, entt::entity other) = 0;
        virtual void OnDestroyed(entt::entity self) = 0;
        virtual std::unique_ptr<IScript> clone() const = 0;
    };

private:
    std::unique_ptr<IScript> script;

    template<typename T>
    class ScriptModel : public IScript {        
        T scriptImpl;

    public:
        ScriptModel(T script) : scriptImpl(std::move(script)) {}

        void OnConstructed(entt::entity self) override {
            scriptImpl.OnConstructed(self);
        }

        void OnEvent(entt::entity self, const SDL_Event& event) override {
            scriptImpl.OnEvent(self, event);
        }

        void OnUpdate(entt::entity self, float dt) override {
            scriptImpl.OnUpdate(self, dt);
        }

        void OnCollision(entt::entity self, entt::entity other) override {
            scriptImpl.OnCollision(self, other);
        }

        void OnDestroyed(entt::entity self) override {
            scriptImpl.OnDestroyed(self);
        }

        std::unique_ptr<IScript> clone() const override {
            return std::make_unique<ScriptModel>(*this);
        }
    };

public:
    template<typename T>
    ScriptComponent(T script) : script(std::make_unique<ScriptModel<T>>(std::move(script))) {}

    ScriptComponent(const ScriptComponent& other) : script(other.script ? other.script->clone() : nullptr) {}
 
    ScriptComponent& operator=(const ScriptComponent& other) {
        if (this != &other) {
            script = other.script ? other.script->clone() : nullptr;
        }
        return *this;
    }

    void OnConstructed(entt::entity self) {
        if (script) script->OnConstructed(self);
    }

    void OnEvent(entt::entity self, const SDL_Event& event) {
        if (script) script->OnEvent(self, event);
    }

    void OnUpdate(entt::entity self, float dt) {
        if (script) script->OnUpdate(self, dt);
    }

    void OnCollision(entt::entity self, entt::entity other) {
        if (script) script->OnCollision(self, other);
    }

    void OnDestroyed(entt::entity self) {
        if (script) script->OnDestroyed(self);
    }

    ~ScriptComponent() {
        script.release();
    }
};
