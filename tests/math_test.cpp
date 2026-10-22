#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <vendor/doctest/doctest.h>

#include "core/math.hpp"

namespace {

constexpr float PI = 3.14159265358979323846f;

void check_vec2(const varicle::Vec2& v, float x, float y) {
    CHECK(v.x == doctest::Approx(x));
    CHECK(v.y == doctest::Approx(y));
}

void check_vec3(const varicle::Vec3& v, float x, float y, float z) {
    CHECK(v.x == doctest::Approx(x));
    CHECK(v.y == doctest::Approx(y));
    CHECK(v.z == doctest::Approx(z));
}

void check_mat2(const varicle::Mat2& m, const varicle::Mat2& e) {
    for (int c = 0; c < 2; ++c) {
        CHECK(m[c][0] == doctest::Approx(e[c][0]));
        CHECK(m[c][1] == doctest::Approx(e[c][1]));
    }
}

void check_mat3(const varicle::Mat3& m, const varicle::Mat3& e) {
    for (int c = 0; c < 3; ++c) {
        for (int r = 0; r < 3; ++r) {
            CHECK(m[c][r] == doctest::Approx(e[c][r]));
        }
    }
}

void check_mat4(const varicle::Mat4& m, const varicle::Mat4& e) {
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            CHECK(m[c][r] == doctest::Approx(e[c][r]));
        }
    }
}

} // namespace

TEST_CASE("Mat2 basic operations") {
    using namespace varicle;

    check_mat2(Mat2::identity(), { Vec2{ 1.0f, 0.0f }, Vec2{ 0.0f, 1.0f } });
    check_mat2(Mat2::from(3.0f), { Vec2{ 3.0f, 0.0f }, Vec2{ 0.0f, 3.0f } });

    Mat2 m{ Vec2{ 1.0f, 2.0f }, Vec2{ 3.0f, 4.0f } };
    check_mat2(m * Mat2::identity(), m);

    Vec2 v = m * Vec2{ 5.0f, 6.0f };
    check_vec2(v, 23.0f, 34.0f);

    check_mat2(m + m, { Vec2{ 2.0f, 4.0f }, Vec2{ 6.0f, 8.0f } });
    check_mat2(m - m, Mat2::identity() * 0.0f);
}

TEST_CASE("Mat3 basic operations") {
    using namespace varicle;

    check_mat3(Mat3::identity(), { Vec3{ 1.0f, 0.0f, 0.0f },
                                   Vec3{ 0.0f, 1.0f, 0.0f },
                                   Vec3{ 0.0f, 0.0f, 1.0f } });
    check_mat3(Mat3::from(2.0f), { Vec3{ 2.0f, 0.0f, 0.0f },
                                   Vec3{ 0.0f, 2.0f, 0.0f },
                                   Vec3{ 0.0f, 0.0f, 2.0f } });

    Mat3 m{ Vec3{ 1.0f, 2.0f, 3.0f },
            Vec3{ 4.0f, 5.0f, 6.0f },
            Vec3{ 7.0f, 8.0f, 9.0f } };
    check_mat3(m * Mat3::identity(), m);

    Vec3 v = m * Vec3{ 1.0f, 2.0f, 3.0f };
    check_vec3(v, 30.0f, 36.0f, 42.0f);
}

TEST_CASE("Mat4 basic operations") {
    using namespace varicle;

    check_mat4(Mat4::identity(), { Vec4{ 1.0f, 0.0f, 0.0f, 0.0f },
                                   Vec4{ 0.0f, 1.0f, 0.0f, 0.0f },
                                   Vec4{ 0.0f, 0.0f, 1.0f, 0.0f },
                                   Vec4{ 0.0f, 0.0f, 0.0f, 1.0f } });
    check_mat4(Mat4::from(2.0f), { Vec4{ 2.0f, 0.0f, 0.0f, 0.0f },
                                   Vec4{ 0.0f, 2.0f, 0.0f, 0.0f },
                                   Vec4{ 0.0f, 0.0f, 2.0f, 0.0f },
                                   Vec4{ 0.0f, 0.0f, 0.0f, 2.0f } });

    Mat4 m{ Vec4{ 1.0f, 2.0f, 3.0f, 4.0f },
            Vec4{ 5.0f, 6.0f, 7.0f, 8.0f },
            Vec4{ 9.0f, 10.0f, 11.0f, 12.0f },
            Vec4{ 13.0f, 14.0f, 15.0f, 16.0f } };
    check_mat4(m * Mat4::identity(), m);

    Vec4 v = m * Vec4{ 1.0f, 2.0f, 3.0f, 4.0f };
    check_vec2(Vec2{ v.x, v.y }, 90.0f, 100.0f);
    check_vec2(Vec2{ v.z, v.w }, 110.0f, 120.0f);
}

TEST_CASE("2D rotation") {
    using namespace varicle;

    check_mat2(rotate(0.0f), Mat2::identity());
    check_mat2(rotate(PI / 2.0f) * rotate(-PI / 2.0f), Mat2::identity());

    check_vec2(rotate(PI / 2.0f) * Vec2::RIGHT(), 0.0f, 1.0f);
    check_vec2(rotate(PI / 2.0f) * Vec2::UP(), -1.0f, 0.0f);
    check_vec2(rotate(PI / 4.0f) * Vec2::RIGHT(), 0.70710678f, 0.70710678f);

    Mat2 m{ Vec2{ 1.0f, 2.0f }, Vec2{ 3.0f, 4.0f } };
    check_mat2(rotate(m, PI / 2.0f), m * rotate(PI / 2.0f));
}

TEST_CASE("3D rotation axes") {
    using namespace varicle;

    check_mat3(rotate_x(0.0f), Mat3::identity());
    check_mat3(rotate_y(0.0f), Mat3::identity());
    check_mat3(rotate_z(0.0f), Mat3::identity());

    check_vec3(rotate_x(PI / 2.0f) * Vec3{ 0.0f, 1.0f, 0.0f }, 0.0f, 0.0f, 1.0f);
    check_vec3(rotate_x(PI / 2.0f) * Vec3{ 0.0f, 0.0f, 1.0f }, 0.0f, -1.0f, 0.0f);

    check_vec3(rotate_y(PI / 2.0f) * Vec3{ 0.0f, 0.0f, 1.0f }, 1.0f, 0.0f, 0.0f);
    check_vec3(rotate_y(PI / 2.0f) * Vec3{ 1.0f, 0.0f, 0.0f }, 0.0f, 0.0f, -1.0f);

    check_vec3(rotate_z(PI / 2.0f) * Vec3{ 1.0f, 0.0f, 0.0f }, 0.0f, 1.0f, 0.0f);
    check_vec3(rotate_z(PI / 2.0f) * Vec3{ 0.0f, 1.0f, 0.0f }, -1.0f, 0.0f, 0.0f);

    check_mat3(rotate_x(PI / 3.0f) * rotate_x(-PI / 3.0f), Mat3::identity());
    check_mat3(rotate_y(PI / 3.0f) * rotate_y(-PI / 3.0f), Mat3::identity());
    check_mat3(rotate_z(PI / 3.0f) * rotate_z(-PI / 3.0f), Mat3::identity());
}

TEST_CASE("3D rotation matrix overloads") {
    using namespace varicle;

    Mat3 m{ Vec3{ 1.0f, 2.0f, 3.0f },
            Vec3{ 4.0f, 5.0f, 6.0f },
            Vec3{ 7.0f, 8.0f, 9.0f } };

    check_mat3(rotate_x(m, PI / 2.0f), m * rotate_x(PI / 2.0f));
    check_mat3(rotate_y(m, PI / 2.0f), m * rotate_y(PI / 2.0f));
    check_mat3(rotate_z(m, PI / 2.0f), m * rotate_z(PI / 2.0f));

    check_mat3(rotate_z(Mat3::identity(), PI / 2.0f), rotate_z(PI / 2.0f));
}

TEST_CASE("Translation, scale and model matrix") {
    using namespace varicle;

    check_vec3(translate(Vec3{ 1.0f, 2.0f, 3.0f }) * Vec3{ 0.0f, 0.0f, 0.0f },
               1.0f, 2.0f, 3.0f);
    check_vec3(scale(Vec3{ 2.0f, 3.0f, 4.0f }) * Vec3{ 1.0f, 1.0f, 1.0f },
               2.0f, 3.0f, 4.0f);

    Transform3D identity;
    check_mat4(get_model_matrix(identity), Mat4::identity());

    Transform3D transform;
    transform.position = { 1.0f, 2.0f, 3.0f };
    transform.scale    = { 2.0f, 3.0f, 4.0f };
    transform.rotation = { 0.0f, 0.0f, 0.0f };
    check_vec3(get_model_matrix(transform) * Vec3{ 1.0f, 1.0f, 1.0f },
               3.0f, 5.0f, 7.0f);

    Transform3D rotated;
    rotated.scale    = { 1.0f, 1.0f, 1.0f };
    rotated.rotation = { 0.0f, 0.0f, PI / 2.0f };
    check_vec3(get_model_matrix(rotated) * Vec3{ 1.0f, 0.0f, 0.0f },
               0.0f, 1.0f, 0.0f);
}

TEST_CASE("Vector math helpers") {
    using namespace varicle;

    CHECK(dot(Vec2{ 1.0f, 2.0f }, Vec2{ 3.0f, 4.0f }) == doctest::Approx(11.0f));
    CHECK(dot(Vec3{ 1.0f, 2.0f, 3.0f }, Vec3{ 4.0f, 5.0f, 6.0f }) ==
          doctest::Approx(32.0f));

    check_vec3(cross(Vec3{ 1.0f, 0.0f, 0.0f }, Vec3{ 0.0f, 1.0f, 0.0f }),
               0.0f, 0.0f, 1.0f);

    check_vec2(normalize(Vec2{ 3.0f, 4.0f }), 0.6f, 0.8f);
    check_vec3(normalize(Vec3{ 1.0f, 2.0f, 2.0f }), 1.0f / 3.0f, 2.0f / 3.0f,
               2.0f / 3.0f);
}
