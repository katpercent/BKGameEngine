#ifndef VEC4_HPP
#define VEC4_HPP

#include <cmath>
#include <iostream>
#include <limits>

namespace math {

#ifdef __OPENCL_VERSION__  

    // ========================
    // OpenCL : alias float4
    // ========================
    typedef float4 vec4;

#else  

    // =============
    // CPU : struct
    // =============
    struct alignas(16) vec4 {
        float x, y, z, w;

        // Constructors
        inline vec4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        inline explicit vec4(float v) : x(v), y(v), z(v), w(1.0f) {}
        inline vec4(float x, float y, float z, float w = 1.0f) : x(x), y(y), z(z), w(w) {}
        inline vec4(const vec4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

        // Operators
        inline vec4 operator+(const vec4& other) const {
            return vec4(x + other.x, y + other.y, z + other.z, w);
        }

        inline vec4 operator-(const vec4& other) const {
            return vec4(x - other.x, y - other.y, z - other.z, w);
        }

        inline vec4 operator-() const {
            return vec4(-x, -y, -z, w);
        }

        inline vec4 operator*(float scalar) const {
            return vec4(x * scalar, y * scalar, z * scalar, w);
        }

        inline vec4 operator/(float scalar) const {
            if (std::abs(scalar) < std::numeric_limits<float>::epsilon())
                return *this;
            return vec4(x / scalar, y / scalar, z / scalar, w);
        }

        // Custom operator*
        inline vec4 operator*(const vec4& other) const {
            float newX = x * other.x + x * other.y + x * other.z;
            float newY = y * other.x + y * other.y + z * other.z;
            float newZ = z * other.x + z * other.y + z * other.z;
            return vec4(newX, newY, newZ, 1.0f);
        }

        inline bool operator==(const vec4& other) const {
            return (x == other.x && y == other.y && z == other.z && w == other.w);
        }

        inline bool operator!=(const vec4& other) const {
            return !(*this == other);
        }

        // Utilitaires
        inline vec4 lerp(const vec4& other, float t) const {
            return *this * (1.0f - t) + other * t;
        }

        inline float dot(const vec4& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        inline float magnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        inline vec4 normalize() const {
            float mag = magnitude();
            return (mag > 0.0f) ? (*this / mag) : *this;
        }

        inline vec4 reflect(const vec4& normal) const {
            float dotProduct = this->dot(normal);
            if (std::abs(dotProduct) < 0.001f) {
                return -*this;
            }
            return *this - normal * 2.0f * dotProduct;
        }

        inline vec4 crossProduct(const vec4& other) const {
            float crossX = y * other.z - z * other.y;
            float crossY = z * other.x - x * other.z;
            float crossZ = x * other.y - y * other.x;
            return vec4(crossX, crossY, crossZ, w);
        }

        // Swizzling
        inline vec4 xyzw() const { return *this; }
        inline vec4 xyz() const { return vec4(x, y, z, 0.0f); }
        inline vec4 xy() const { return vec4(x, y, 0.0f, 0.0f); }

        // Output
        friend inline std::ostream& operator<<(std::ostream& os, const vec4& v) {
            os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
            return os;
        }
    };

    // Scalar * vec4
    inline vec4 operator*(float scalar, const vec4& v) {
        return v * scalar;
    }

#endif // __OPENCL_VERSION__
} // namespace math
#endif // VEC4_HPP

