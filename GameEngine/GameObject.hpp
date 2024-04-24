#pragma once
#include <entt.hpp>
#include "Registry.hpp"

class GameObject {
public:
    // Constructor to create a new entity
    GameObject(): m_entity(Registry::Get().create()) {}

    // Constructor to use an existing entity
    GameObject(entt::registry& reg, entt::entity entity): m_entity(entity) {}

    // Template method to add or replace a component
    template<typename T, typename... Args>
    GameObject& AddComponent(Args&&... args) {
        Registry::Get().emplace_or_replace<T>(m_entity, std::forward<Args>(args)...);
        return *this;
    }

    // Template method to get a component
    template<typename T>
    T& GetComponent() {
        return Registry::Get().get<T>(m_entity);
    }

    // Template method to check if a component exists
    template<typename T>
    bool HasComponent() const {
        return Registry::Get().any_of<T>(m_entity);
    }

    // Template method to remove a component
    template<typename T>
    GameObject& RemoveComponent() {
        if(Registry::Get().any_of<T>(m_entity)) {
            Registry::Get().erase<T>(m_entity);
        }
        return *this;
    }

    void Destroy() {
        if(Registry::Get().valid(m_entity)) {
            Registry::Get().destroy(m_entity);
        }
    }

    bool IsValid() {
        return Registry::Get().valid(m_entity);
    }

private:
    entt::entity m_entity;
};
