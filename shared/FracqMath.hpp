#pragma once

namespace Math {
inline float FastSqrt(float number) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y = number;
    i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y));

    return number * y;
}

struct Vector2 {
    float x, y;

    Vector2()
        : x(0)
        , y(0) { }
    Vector2(float x, float y)
        : x(x)
        , y(y) { }

    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }

    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }

    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }

    Vector2 operator-() const { return Vector2(-x, -y); }

    float Length() const { return FastSqrt(x * x + y * y); }

    float DistanceTo(const Vector2& other) const {
        Vector2 diff = *this - other;
        return diff.Length();
    }

    Vector2 Normalized() const {
        float len = Length();
        if (len > 0.0001f) {
            return Vector2(x / len, y / len);
        } else {
            return Vector2(0, 0);
        }
    }
};

struct Vector3 {
    float x, y, z;

    Vector3()
        : x(0)
        , y(0)
        , z(0) { }
    Vector3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z) { }

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    float Length() const { return FastSqrt(x * x + y * y + z * z); }

    float DistanceTo(const Vector3& other) const {
        Vector3 diff = *this - other;
        return diff.Length();
    }

    Vector3 Normalized() const {
        float len = Length();
        if (len > 0.0001f) {
            return Vector3(x / len, y / len, z / len);
        } else {
            return Vector3(0, 0, 0);
        }
    }
};

struct Vector4 {
    float x, y, z, w;

    Vector4()
        : x(0)
        , y(0)
        , z(0)
        , w(0) { }
    Vector4(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w) { }

    Vector4 operator+(const Vector4& other) const {
        return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Vector4 operator-(const Vector4& other) const {
        return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Vector4 operator*(float scalar) const {
        return Vector4(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

    float Length() const { return FastSqrt(x * x + y * y + z * z + w * w); }

    float DistanceTo(const Vector4& other) const {
        Vector4 diff = *this - other;
        return diff.Length();
    }

    Vector4 Normalized() const {
        float len = Length();
        if (len > 0.0001f) {
            return Vector4(x / len, y / len, z / len, w / len);
        } else {
            return Vector4(0, 0, 0, 0);
        }
    }
};
}