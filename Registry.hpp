#pragma once
#include <entity/registry.hpp>

class Registry {
public:
    // Get a reference to the shared EnTT registry
    static entt::registry& Get() {
        static entt::registry instance;
        return instance;
    }

    // Delete copy constructor and copy assignment operator to prevent copying
    Registry(const Registry&) = delete;
    Registry& operator=(const Registry&) = delete;

    // Delete move constructor and move assignment operator to prevent moving
    Registry(Registry&&) = delete;
    Registry& operator=(Registry&&) = delete;

private:
    // Private constructor to prevent instantiation
    Registry() {}
};
