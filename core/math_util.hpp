#pragma once

#include <random>

namespace varicle {
// Generate a random float between min and max
inline float RandomRange(float min, float max) {
    static std::random_device             rd;
    static std::mt19937                   gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

// Generate a random int between min and max
inline int RandomRange(int min, int max) {
    static std::random_device          rd;
    static std::mt19937                gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
} // namespace varicle
