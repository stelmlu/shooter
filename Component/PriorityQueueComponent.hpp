#pragma once

#include <vector>
#include <algorithm>
#include <utility> // For std::pair

template<typename T>
class PriorityQueueComponent {
public:
    using PairType = std::pair<int, T>;
    using ContainerType = std::vector<PairType>;
    using Iterator = typename ContainerType::iterator;

private:
    ContainerType elements;

    // Helper function to sort the elements in descending order by their int value
    void sortElements() {
        std::sort(elements.begin(), elements.end(), [](const PairType& a, const PairType& b) {
            return a.first > b.first; // Sort in descending order of the int value
        });
    }

public:
    // Inserts a new element into the priority queue and sorts the container
    PriorityQueueComponent<T>& Add(T&& value, int priority = 0) {
        elements.emplace_back(priority, std::forward<T>(value));
        sortElements();
        return *this;
    }

    // Returns an iterator to the beginning of the container
    Iterator begin() {
        return elements.begin();
    }

    // Returns an iterator to the end of the container
    Iterator end() {
        return elements.end();
    }
};
