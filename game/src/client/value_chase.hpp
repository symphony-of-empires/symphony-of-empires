#pragma once

#include <chrono>
#include <glm/common.hpp>

template <typename T>
class ValueChase {
    float friction;
    std::chrono::system_clock::time_point last_time;
public:
    ValueChase(float _friction): friction{ _friction } {};

    T move_toward(T current, T target) {
        auto now = std::chrono::system_clock::now();
        float time = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count();
        time /= 1e3;
        time *= (1.f / friction);
        time = glm::min(time, 1.f);
        last_time = now;
        T distance = target - current;
        T step = distance * time;
        // const float tolerance = 10.f;
        // T new_current = glm::round((current + step) * tolerance) / tolerance;
        return current + step;
    }
};