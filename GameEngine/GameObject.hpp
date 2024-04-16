#pragma once
#include <entt.hpp>
#include "Registry.hpp"

class GameObject {
public:
    // Constructor to create a new entity
    GameObject()
        : m_registry(Registry::Get()), m_entity(m_registry.create()) {}

    // Constructor to use an existing entity
    GameObject(entt::registry& reg, entt::entity entity)
        : m_registry(reg), m_entity(entity) {}

    // Template method to add or replace a component
    template<typename T, typename... Args>
    GameObject& AddComponent(Args&&... args) {
        m_registry.emplace_or_replace<T>(m_entity, std::forward<Args>(args)...);
        return *this;
    }

    // Template method to get a component
    template<typename T>
    T& GetComponent() {
        return m_registry.get<T>(m_entity);
    }

    // Template method to check if a component exists
    template<typename T>
    bool HasComponent() const {
        return m_registry.any_of<T>(m_entity);
    }

    // Template method to remove a component
    template<typename T>
    GameObject& RemoveComponent() {
        m_registry.erase<T>(m_entity);
        return *this;
    }

    void Destroy() {
        if(m_registry.valid(m_entity)) {
            m_registry.destroy(m_entity);
        }
    }

    bool IsValid() {
        return m_registry.valid(m_entity);
    }

private:
    entt::registry& m_registry;
    entt::entity m_entity;
};
