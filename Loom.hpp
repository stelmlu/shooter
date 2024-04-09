#pragma once
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <typeindex>
#include <memory>
#include <algorithm>

using EntityId = uint32_t;

class World {
private:
    struct QueryCacheKey {
        std::array<size_t, 16> key{};
        size_t length = 0; // Keep track of the actual number of types stored

        QueryCacheKey(const std::initializer_list<std::type_index>& types) {
            for (const auto& type : types) {
                if (length < key.size()) {
                    key[length++] = type.hash_code();
                } else {
                    // Handle the error or ignore if exceeding the maximum allowed component types
                    throw std::runtime_error("maximum number of component in the query, the limit is 16.");
                    break;
                }
            }
        }

        bool operator==(const QueryCacheKey& other) const {
            if (length != other.length) return false;
            for (size_t i = 0; i < length; ++i) {
                if (key[i] != other.key[i]) return false;
            }
            return true;
        }
    };

    struct QueryCacheKeyHash {
        std::size_t operator()(const QueryCacheKey& k) const {
            std::size_t hash = 0;
            for (size_t i = 0; i < k.length; ++i) {
                hash ^= std::hash<size_t>{}(k.key[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

    struct PairHash {
        template <typename TFirst, typename TSecond>
        std::size_t operator()(const std::pair<TFirst, TSecond>& pair) const {
            auto hash1 = std::hash<TFirst>{}(pair.first);
            auto hash2 = std::hash<TSecond>{}(pair.second);
            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };


    EntityId nextId = 1;
    std::unordered_map<EntityId, std::unordered_map<std::type_index, size_t>> idToComponentIndex;
    std::unordered_map<std::type_index, std::vector<uint8_t>> componentStorage;
    std::unordered_map<std::type_index, size_t> componentSizes;
    std::unordered_map<std::type_index, std::unordered_set<EntityId>> typeToEntities;
    std::unordered_map<std::type_index, std::function<void*(void*)>> copyConstructors;
    std::unordered_map<std::type_index, void(*)(void*)> destructors;
    std::unordered_map<QueryCacheKey, std::vector<EntityId>, QueryCacheKeyHash> queryCache;
    std::unordered_map<std::pair<EntityId, std::type_index>, uint8_t*, PairHash> componentDataCache;
    std::unordered_set<std::pair<EntityId, EntityId>, PairHash> connectionLookup;
    std::unordered_map<EntityId, std::unordered_set<EntityId>> connectionsMap;

    void addComponentDirectly(EntityId entityId, std::type_index typeIndex, void* componentPtr, size_t componentSize) {
        if (componentStorage.find(typeIndex) == componentStorage.end()) {
            componentStorage[typeIndex] = std::vector<uint8_t>();
        }

        // Append the component to the storage vector
        size_t index = componentStorage[typeIndex].size();
        componentStorage[typeIndex].resize(index + componentSize);
        std::memcpy(componentStorage[typeIndex].data() + index, componentPtr, componentSize);

        // Update indexing to associate the new component with the new entity
        if (idToComponentIndex.find(entityId) == idToComponentIndex.end()) {
            idToComponentIndex[entityId] = std::unordered_map<std::type_index, size_t>();
        }
        idToComponentIndex[entityId][typeIndex] = index / componentSize;
    }

    template<typename T>
    static void destroyComponent(void* data) {
        reinterpret_cast<T*>(data)->~T();
    }

    void clearComponents() {
        for (auto& [type, storage] : componentStorage) {
            auto destructor = destructors[type];
            size_t componentSize = componentSizes[type];
            for (size_t i = 0; i < storage.size(); i += componentSize) {
                destructor(storage.data() + i);
            }
        }
    }

    template<typename T>
    void storeCopyConstructor() {
        copyConstructors[std::type_index(typeid(T))] = [](void* src) -> void* {
            T* source = static_cast<T*>(src);
            return new T(*source); // Invoke the copy constructor of T
        };
    }

    void removeAllConnection(EntityId id) {
        // Remove all entries in the lookup table for the given ID
        if (connectionsMap.find(id) != connectionsMap.end()) {
            for (const int to : connectionsMap[id]) {
                connectionLookup.erase({id, to});
            }
            connectionsMap.erase(id);
        }

        // Also, need to remove this ID from all other connections
        for (auto& [from, toSet] : connectionsMap) {
            if (toSet.erase(id) > 0) {
                // If 'id' was indeed connected to 'from', remove it from the lookup as well
                connectionLookup.erase({from, id});
            }
        }
    }

    static std::pair<EntityId, std::type_index> makeCacheKey(EntityId id, std::type_index type) {
        return {id, type};
    }

public:
    World() = default;

    ~World() {
        clearComponents();
    }

    EntityId CreateEntity() {
        return nextId++;
    }

    EntityId CloneEntity(EntityId sourceId) {
        EntityId newId = CreateEntity();

        // Copy the component
        for (auto& [typeIndex, componentIndex] : idToComponentIndex[sourceId]) {
            typeToEntities[typeIndex].emplace(newId);

            auto& srcComponentStorage = componentStorage[typeIndex];
            size_t componentSize = componentSizes[typeIndex];

            size_t offset = componentIndex * componentSize;
            void* srcComponentPtr = static_cast<void*>(srcComponentStorage.data() + offset);
            void* newComponentPtr = copyConstructors[typeIndex](srcComponentPtr);
            addComponentDirectly(newId, typeIndex, newComponentPtr, componentSize);
        }

        // Copy the connection
        auto connectionFindResult = connectionsMap.find(sourceId);
        if(connectionFindResult != connectionsMap.end()) {
            for(auto toId : connectionFindResult->second) {
                Connect(newId, toId);
            }
        }

        queryCache.clear(); // Invalidate the cash

        return newId;
    }

    void DestroyEntity(EntityId id) {
        for (auto& [type, index] : idToComponentIndex[id]) {
            auto& storage = componentStorage[type];
            size_t size = componentSizes[type];
            size_t offset = index * size;
            destructors[type](storage.data() + index);
            storage.erase(storage.begin() + offset, storage.begin() + offset + size);
            typeToEntities[type].erase(id);
            
            // Remove the component from component data cache.
            auto cacheKey = makeCacheKey(id, type);
            if(auto it = componentDataCache.find(cacheKey); it != componentDataCache.end()) {
                componentDataCache.erase(it);
            }
        }
        idToComponentIndex.erase(id);
        queryCache.clear(); // Invalidate the cash
        removeAllConnection(id);
    }

    template<typename T>
    T& EmplaceComponent(EntityId id, T&& component) {
        auto typeIndex = std::type_index(typeid(T));
        auto& indexMap = idToComponentIndex[id];
        uint8_t* data;
        if (indexMap.find(typeIndex) != indexMap.end()) {
            size_t index = indexMap[typeIndex];
            data = componentStorage[typeIndex].data() + index * sizeof(T);
            destroyComponent<T>(data);
            new (componentStorage[typeIndex].data() + index * sizeof(T)) T(std::forward<T>(component));
        } else {
            indexMap[typeIndex] = componentStorage[typeIndex].size() / sizeof(T);
            componentStorage[typeIndex].resize(componentStorage[typeIndex].size() + sizeof(T));
            size_t index = indexMap[typeIndex];
            data = componentStorage[typeIndex].data() + index * sizeof(T);
            new (data) T(std::forward<T>(component));
            typeToEntities[typeIndex].emplace(id);
            componentSizes[std::type_index(typeid(T))] = sizeof(T);
            if (copyConstructors.find(std::type_index(typeid(T))) == copyConstructors.end()) {
                storeCopyConstructor<T>();
            }
            if (destructors.find(std::type_index(typeid(T))) == destructors.end()) {
                destructors[typeIndex] = &destroyComponent<T>;
            }
            queryCache.clear(); // Invalidate the cashe
        }
        return *reinterpret_cast<T*>(data);
    }

    template<typename T>
    void RemoveComponent(EntityId id) {
        auto typeIndex = std::type_index(typeid(T));
        if (idToComponentIndex[id].find(typeIndex) != idToComponentIndex[id].end()) {
            size_t index = idToComponentIndex[id][typeIndex];
            auto& storage = componentStorage[typeIndex];
            destroyComponent<T>(storage.data() + index * sizeof(T));
            idToComponentIndex[id].erase(typeIndex);
            storage.erase(storage.begin() + index * sizeof(T), storage.begin() + (index+1) * sizeof(T));
            typeToEntities[typeIndex].erase(id);
            queryCache.clear(); // Invalidate the cashe

            // Remove the component from component data cache.
            auto cacheKey = makeCacheKey(id, typeIndex);
            if(auto it = componentDataCache.find(cacheKey); it != componentDataCache.end()) {
                componentDataCache.erase(it);
            }
        }
    }

    template<typename T>
    T& GetComponent(EntityId id) {
        auto typeIndex = std::type_index(typeid(T));
        auto cacheKey = makeCacheKey(id, typeIndex);

        // Check if the component is in the cache
        if(auto it = componentDataCache.find(cacheKey); it != componentDataCache.end()) {
            return *reinterpret_cast<T*>(it->second);
        }

        // Cache miss, add the data to the componets cache
        size_t index = idToComponentIndex[id][typeIndex];
        uint8_t* data = componentStorage[typeIndex].data() + index * sizeof(T);
        componentDataCache[cacheKey] = data;

        return *reinterpret_cast<T*>(data);
    }

    template<typename T>
    bool HasComponent(EntityId id) {
        auto typeIndex = std::type_index(typeid(T));
        return idToComponentIndex[id].find(typeIndex) != idToComponentIndex[id].end();
    }

    template<typename... Components>
    std::vector<EntityId> Query() {
        QueryCacheKey key({std::type_index(typeid(Components))...});

        // Check if the result is cached
        auto it = queryCache.find(key);
        if (it != queryCache.end()) {
            return it->second;
        }

        // Array to store the size and corresponding entities set for each component
        std::array<std::pair<size_t, const std::unordered_set<EntityId>*>, sizeof...(Components)> componentEntities;

        // Initialize the array with the sizes and pointers to each component's entities set
        size_t index = 0;
        ((componentEntities[index++] = std::make_pair(typeToEntities[std::type_index(typeid(Components))].size(), &typeToEntities[std::type_index(typeid(Components))])), ...);

        // Sort the array based on the size, so the smallest set is first
        std::sort(componentEntities.begin(), componentEntities.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.first < rhs.first;
        });

        // Now iterate through the smallest set and check if those entities have all the other components
        std::vector<EntityId> result;
        const auto& [_, smallestSet] = componentEntities.front();
        for (EntityId entityId : *smallestSet) {
            bool hasAllComponents = true;

            // Check if this entity has all the other components
            for (size_t i = 1; i < sizeof...(Components); ++i) {
                const auto& [_, entitiesSet] = componentEntities[i];
                if (entitiesSet->find(entityId) == entitiesSet->end()) {
                    hasAllComponents = false;
                    break;
                }
            }

            if (hasAllComponents) {
                result.push_back(entityId);
            }
        }

        // Cache the result
        queryCache[key] = result;

        return result;
    }

    void Connect(EntityId from, EntityId to) {
        connectionLookup.insert({from, to});
        connectionsMap[from].insert(to);
    }

    void Disconnect(EntityId from, EntityId to) {
        connectionLookup.erase({from, to});
        if (connectionsMap.find(from) != connectionsMap.end()) {
            connectionsMap[from].erase(to);
        }
    }

    bool IsConnected(EntityId from, EntityId to) {
        return connectionLookup.find({from, to}) != connectionLookup.end();
    }
};
