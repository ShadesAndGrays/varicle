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

    Mat4 matrix{};

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

inline Mat4 translate(const Vec3& v) {
    Mat4 r = Mat4::identity();
    r[3]   = Vec4{ v.x, v.y, v.z, 1.0f };
    return r;
}

inline Mat4 scale(const Vec3& v) {
    Mat4 r  = Mat4::identity();
    r[0][0] = v.x;
    r[1][1] = v.y;
    r[2][2] = v.z;
    return r;
}

inline Mat4 get_model_matrix(const Transform3D& t) {
    Mat3 rotation = rotate_x(t.rotation.x) * rotate_y(t.rotation.y) *
        rotate_z(t.rotation.z);

    Mat4 r = Mat4::identity();
    for (int c = 0; c < 3; ++c) {
        for (int row = 0; row < 3; ++row) {
            r[c][row] = rotation[c][row];
        }
    }

    return translate(t.position) * r * scale(t.scale);
}

// --- ZERO-TO-ONE DEPTH RANGE ([0, 1]) & Y-DOWN NDC ---
inline Mat4 perspective_zero_to_one(
    float fov_radians,
    float aspect,
    float near_plane,
    float far_plane
) {
    float tan_half_fov = std::tan(fov_radians / 2.0f);
    Mat4  matrix{};

    matrix.m[0]  = 1.0f / (aspect * tan_half_fov);
    matrix.m[5]  = -1.0f / tan_half_fov; // Inverted Y for Y-down NDC
    matrix.m[10] = far_plane / (near_plane - far_plane);
    matrix.m[11] = -1.0f;
    matrix.m[14] = (far_plane * near_plane) / (near_plane - far_plane);

    return matrix;
}

inline Mat4 ortho_zero_to_one(
    float left,
    float right,
    float bottom,
    float top,
    float near_plane,
    float far_plane
) {
    Mat4 matrix{};

    matrix.m[0]  = 2.0f / (right - left);
    matrix.m[5]  = -2.0f / (top - bottom); // Inverted Y for Y-down NDC
    matrix.m[10] = 1.0f / (near_plane - far_plane);
    matrix.m[12] = -(right + left) / (right - left);
    matrix.m[13] = -(top + bottom) / (top - bottom);
    matrix.m[14] = near_plane / (near_plane - far_plane);
    matrix.m[15] = 1.0f;

    return matrix;
}

// --- NEGATIVE-ONE-TO-ONE DEPTH RANGE ([-1, 1]) & Y-UP NDC ---
inline Mat4 perspective_negative_one_to_one(
    float fov_radians,
    float aspect,
    float near_plane,
    float far_plane
) {
    float tan_half_fov = std::tan(fov_radians / 2.0f);
    Mat4  matrix{};

    matrix.m[0]  = 1.0f / (aspect * tan_half_fov);
    matrix.m[5]  = 1.0f / tan_half_fov; // Standard Y-up NDC
    matrix.m[10] = -(far_plane + near_plane) / (far_plane - near_plane);
    matrix.m[11] = -1.0f;
    matrix.m[14] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);

    return matrix;
}

inline Mat4 ortho_negative_one_to_one(
    float left,
    float right,
    float bottom,
    float top,
    float near_plane,
    float far_plane
) {
    Mat4 matrix{};

    matrix.m[0]  = 2.0f / (right - left);
    matrix.m[5]  = 2.0f / (top - bottom); // Standard Y-up NDC
    matrix.m[10] = -2.0f / (far_plane - near_plane);
    matrix.m[12] = -(right + left) / (right - left);
    matrix.m[13] = -(top + bottom) / (top - bottom);
    matrix.m[14] = -(far_plane + near_plane) / (far_plane - near_plane);
    matrix.m[15] = 1.0f;

    return matrix;
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
