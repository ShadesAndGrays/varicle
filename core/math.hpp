#pragma once

#include "core/type.hpp"
#include <cmath>
#include <random>

namespace varicle {

inline float dot(const Vec2& a, const Vec2& b) {
    return (a.x * b.x) + (a.y * b.y);
}
inline float dot(const Vec3& a, const Vec3& b) {
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline Vec3 normalize(const Vec3& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length > 0.0f) {
        return { v.x / length, v.y / length, v.z / length };
    }
    return { 0.0f, 0.0f, 0.0f };
}

inline Vec2 normalize(const Vec2& v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y);
    if (length > 0.0f) {
        return { v.x / length, v.y / length };
    }
    return { 0.0f, 0.0f };
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x),
    };
}

inline Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 zaxis = normalize(eye - target);
    Vec3 xaxis = normalize(cross(up, zaxis));
    Vec3 yaxis = cross(zaxis, xaxis);

    Mat4 matrix;

    // Column 0
    matrix.m[0] = xaxis.x;
    matrix.m[1] = yaxis.x;
    matrix.m[2] = zaxis.x;
    matrix.m[3] = 0.0f;

    // Column 1
    matrix.m[4] = xaxis.y;
    matrix.m[5] = yaxis.y;
    matrix.m[6] = zaxis.y;
    matrix.m[7] = 0.0f;

    // Column 2
    matrix.m[8]  = xaxis.z;
    matrix.m[9]  = yaxis.z;
    matrix.m[10] = zaxis.z;
    matrix.m[11] = 0.0f;

    // Column 3 (Translation)
    matrix.m[12] = -dot(xaxis, eye);
    matrix.m[13] = -dot(yaxis, eye);
    matrix.m[14] = -dot(zaxis, eye);
    matrix.m[15] = 1.0f;

    return matrix;
}

// Rotation matrices (angle in radians, column-major)

// 2D rotation matrix
inline Mat2 rotate(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { Vec2{ c, s }, Vec2{ -s, c } };
}

inline Mat2 rotate(Mat2 mat, float angle) {
    return mat * rotate(angle);
}

inline Mat3 rotate_x(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { Vec3{ 1.0f, 0.0f, 0.0f },
             Vec3{ 0.0f, c, s },
             Vec3{ 0.0f, -s, c } };
}

inline Mat3 rotate_y(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { Vec3{ c, 0.0f, -s },
             Vec3{ 0.0f, 1.0f, 0.0f },
             Vec3{ s, 0.0f, c } };
}

inline Mat3 rotate_z(float angle) {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return { Vec3{ c, s, 0.0f },
             Vec3{ -s, c, 0.0f },
             Vec3{ 0.0f, 0.0f, 1.0f } };
}

inline Mat3 rotate_x(Mat3 mat, float angle) {
    return mat * rotate_x(angle);
}
inline Mat3 rotate_y(Mat3 mat, float angle) {
    return mat * rotate_y(angle);
}
inline Mat3 rotate_z(Mat3 mat, float angle) {
    return mat * rotate_z(angle);
}

// Generate a random float between min and max
inline float random_range_f(float min, float max) {
    static std::random_device             rd;
    static std::mt19937                   gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

// Generate a random int between min and max
inline int random_range_i(int min, int max) {
    static std::random_device          rd;
    static std::mt19937                gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}
} // namespace varicle
