#pragma once
#include <SDL.h>
#include <cstdint>
#include <memory>
#include <entity/registry.hpp>
#include "GameObject.hpp"

struct Script {
    void OnConstructed(GameObject& self) {}
    void OnEvent(GameObject& self, const SDL_Event& event) {}
    void OnUpdate(GameObject& self, float dt) {}
    void OnCollision(GameObject& self, GameObject& other) {}
    void OnDestroyed(GameObject& self) {}
};

class ScriptComponent {
public:
    struct IScript {
        virtual ~IScript() = default;
        virtual void OnConstructed(GameObject& self) = 0;
        virtual void OnEvent(GameObject& self, const SDL_Event& event) = 0;
        virtual void OnUpdate(GameObject& self, float dt) = 0;
        virtual void OnCollision(GameObject& self, GameObject& other) = 0;
        virtual void OnDestroyed(GameObject& self) = 0;
        virtual std::unique_ptr<IScript> clone() const = 0;
    };

private:
    std::unique_ptr<IScript> script;

    template<typename T>
    class ScriptModel : public IScript {        
        T scriptImpl;

    public:
        ScriptModel(T script) : scriptImpl(std::move(script)) {}

        void OnConstructed(GameObject& self) override {
            scriptImpl.OnConstructed(self);
        }

        void OnEvent(GameObject& self, const SDL_Event& event) override {
            scriptImpl.OnEvent(self, event);
        }

        void OnUpdate(GameObject& self, float dt) override {
            scriptImpl.OnUpdate(self, dt);
        }

        void OnCollision(GameObject& self, GameObject& other) override {
            scriptImpl.OnCollision(self, other);
        }

        void OnDestroyed(GameObject& self) override {
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

    void OnConstructed(GameObject& self) {
        if (script) script->OnConstructed(self);
    }

    void OnEvent(GameObject& self, const SDL_Event& event) {
        if (script) script->OnEvent(self, event);
    }

    void OnUpdate(GameObject& self, float dt) {
        if (script) script->OnUpdate(self, dt);
    }

    void OnCollision(GameObject& self, GameObject& other) {
        if (script) script->OnCollision(self, other);
    }

    void OnDestroyed(GameObject& self) {
        if (script) script->OnDestroyed(self);
    }

    ~ScriptComponent() {
        script.release();
    }
};
