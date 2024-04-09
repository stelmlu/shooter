#pragma once
#include <SDL.h>
#include <cstdint>
#include <memory>
#include "../Loom.hpp"

using EntityId = uint32_t;

struct Script {
    void OnEvent(EntityId self, const SDL_Event& event) {}
    void OnUpdate(EntityId self, float dt) {}
    void OnCollision(EntityId self, EntityId other) {}
};

class ScriptComponent {
public:
    struct IScript {
        virtual ~IScript() = default;
        virtual void OnEvent(EntityId self, const SDL_Event& event) = 0;
        virtual void OnUpdate(EntityId self, float dt) = 0;
        virtual void OnCollision(EntityId self, EntityId other) = 0;
        virtual std::unique_ptr<IScript> clone() const = 0;
    };

private:
    std::unique_ptr<IScript> script;

    template<typename T>
    class ScriptModel : public IScript {        
        T scriptImpl;

    public:
        ScriptModel(T script) : scriptImpl(std::move(script)) {}

        void OnEvent(EntityId self, const SDL_Event& event) override {
            scriptImpl.OnEvent(self, event);
        }

        void OnUpdate(EntityId self, float dt) override {
            scriptImpl.OnUpdate(self, dt);
        }

        void OnCollision(EntityId self, uint32_t other) override {
            scriptImpl.OnCollision(self, other);
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

    void OnEvent(EntityId self, const SDL_Event& event) {
        if (script) script->OnEvent(self, event);
    }

    void OnUpdate(uint32_t self, float dt) {
        if (script) script->OnUpdate(self, dt);
    }

    void OnCollision(uint32_t self, uint32_t other) {
        if (script) script->OnCollision(self, other);
    }

    ~ScriptComponent() {
        script.release();
    }
};
