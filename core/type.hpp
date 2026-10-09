#pragma once

#include <array>
namespace varicle {

struct Vec2 {
    union {
        struct {
            float x;
            float y;
        };
        std::array<float, 2> v;
    };

    float&       operator[](int r) { return v[r]; }
    const float& operator[](int r) const { return v[r]; }
    const float* data() const { return v.data(); }

    static Vec2 from(float s) {
        Vec2 r{ s, s };
        return r;
    }

    bool operator==(const Vec2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vec2& o) const { return !(*this == o); }
    Vec2 operator+(const Vec2& o) const { return { x + o.x, y + o.y }; }
    Vec2 operator-(const Vec2& o) const { return { x - o.x, y - o.y }; }
    Vec2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
    Vec2 operator*(int scalar) const { return { x * scalar, y * scalar }; }

    constexpr static Vec2 ZERO() { return Vec2{ 0.0f, 0.0f }; }
    constexpr static Vec2 ONE() { return Vec2{ 1.0f, 1.0f }; }
    constexpr static Vec2 UP() { return Vec2{ 0.0f, 1.0f }; }
    constexpr static Vec2 DOWN() { return Vec2{ 0.0f, -1.0f }; }
    constexpr static Vec2 LEFT() { return Vec2{ -1.0f, 0.0f }; }
    constexpr static Vec2 RIGHT() { return Vec2{ 1.0f, 0.0f }; }
};

struct Vec3 {

    union {
        struct {
            float x;
            float y;
            float z;
        };
        std::array<float, 3> v;
    };

    float&       operator[](int r) { return v[r]; }
    const float& operator[](int r) const { return v[r]; }
    const float* data() const { return v.data(); }

    // 0's but 'z is 1
    static Vec3 from() {
        Vec3 r{ 0, 0, 1 };
        return r;
    }
    static Vec3 from(float s) {
        Vec3 r{ s, s, s };
        return r;
    }

    bool operator==(const Vec3& o) const {
        return x == o.x && y == o.y && z == o.z;
    }
    bool operator!=(const Vec3& o) const { return !(*this == o); }

    Vec3 operator+(const Vec3& o) const {
        return { x + o.x, y + o.y, z + o.z };
    }
    Vec3 operator-(const Vec3& o) const {
        return { x - o.x, y - o.y, z - o.z };
    }
    Vec3 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }
    Vec3 operator*(int scalar) const {
        return { x * scalar, y * scalar, z * scalar };
    }
};

struct Vec4 {

    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        std::array<float, 4> v;
    };

    float&       operator[](int r) { return v[r]; }
    const float& operator[](int r) const { return v[r]; }
    const float* data() const { return v.data(); }

    // 0's but 'w is 1
    static Vec4 from() {
        Vec4 r{ 0, 0, 0, 1 };
        return r;
    }
    static Vec4 from(float s) {
        Vec4 r{ s, s, s, s };
        return r;
    }

    bool operator==(const Vec4& o) const {
        return x == o.x && y == o.y && z == o.z && w == o.w;
    }
    bool operator!=(const Vec4& o) const { return !(*this == o); }
    Vec4 operator+(const Vec4& o) const {
        return { x + o.x, y + o.y, z + o.z, w + o.w };
    }
    Vec4 operator-(const Vec4& o) const {
        return { x - o.x, y - o.y, z - o.z, w - o.w };
    }
    Vec4 operator*(float scalar) const {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }
    Vec4 operator*(int scalar) const {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }
};

struct Mat2 {

    union {
        std::array<Vec2, 2>  cols;
        std::array<float, 4> m;
    };

    Vec2&        operator[](int c) { return cols[c]; }
    const Vec2&  operator[](int c) const { return cols[c]; }
    const float* data() const { return m.data(); }

    static Mat2 identity() {
        Mat2 r{};
        r[0][0] = r[1][1] = 1;
        return r;
    }
    static Mat2 from(float s) {
        Mat2 r{};
        r[0][0] = r[1][1] = s;
        return r;
    }

    Mat2 operator*(float scalar) const {
        return { cols[0] * scalar, cols[1] * scalar };
    }
    Mat2 operator*(int scalar) const {
        return { cols[0] * scalar, cols[1] * scalar };
    }
};

struct Mat3 {

    union {
        std::array<Vec3, 3>  cols;
        std::array<float, 9> m;
    };

    Vec3&        operator[](int c) { return cols[c]; }
    const Vec3&  operator[](int c) const { return cols[c]; }
    const float* data() const { return m.data(); }

    static Mat3 identity() {
        Mat3 r{};
        r[0][0] = r[1][1] = r[2][2] = 1;
        return r;
    }
    static Mat3 from(float s) {
        Mat3 r{};
        r[0][0] = r[1][1] = r[2][2] = s;
        return r;
    }

    Mat3 operator*(float scalar) const {
        return { cols[0] * scalar, cols[1] * scalar, cols[2] * scalar };
    }
    Mat3 operator*(int scalar) const {
        return { cols[0] * scalar, cols[1] * scalar, cols[2] * scalar };
    }
};

struct Mat4 {
    union {
        std::array<Vec4, 4>   cols;
        std::array<float, 16> m;
    };

    Vec4&        operator[](int c) { return cols[c]; }
    const Vec4&  operator[](int c) const { return cols[c]; }
    const float* data() const { return m.data(); }

    static Mat4 identity() {
        Mat4 r{};
        r[0][0] = r[1][1] = r[2][2] = r[3][3] = 1;
        return r;
    }
    static Mat4 from(float s) {
        Mat4 r{};
        r[0][0] = r[1][1] = r[2][2] = r[3][3] = s;
        return r;
    }

    Mat4 operator*(float scalar) const {
        return { cols[0] * scalar,
                 cols[1] * scalar,
                 cols[2] * scalar,
                 cols[3] * scalar };
    }
    Mat4 operator*(int scalar) const {
        return { cols[0] * scalar,
                 cols[1] * scalar,
                 cols[2] * scalar,
                 cols[3] * scalar };
    }
};

static_assert(sizeof(Vec2) == 2 * sizeof(float));
static_assert(sizeof(Vec3) == 3 * sizeof(float));
static_assert(sizeof(Vec4) == 4 * sizeof(float));

static_assert(sizeof(Mat2) == (4 * sizeof(float)));
static_assert(sizeof(Mat3) == (9 * sizeof(float)));
static_assert(sizeof(Mat4) == (16 * sizeof(float)));

// Standalone Interpolation Helpers
inline Vec2 vec2_lerp(const Vec2& s, const Vec2& e, float a) {
    return { s.x + (e.x - s.x) * a, s.y + (e.y - s.y) * a };
}
inline Vec3 vec3_lerp(const Vec3& s, const Vec3& e, float a) {
    return { s.x + (e.x - s.x) * a,
             s.y + (e.y - s.y) * a,
             s.z + (e.z - s.z) * a };
}
inline Vec4 vec4_lerp(const Vec4& s, const Vec4& e, float a) {
    return { s.x + (e.x - s.x) * a,
             s.y + (e.y - s.y) * a,
             s.z + (e.z - s.z) * a,
             s.w + (e.w - s.w) * a };
}
} // namespace varicle
