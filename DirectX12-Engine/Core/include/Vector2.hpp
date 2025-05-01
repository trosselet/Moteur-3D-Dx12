#ifndef VECTOR2__H
#define VECTOR2__H

#include "Define.h"
#include <cmath>
#include "Vector.hpp"

template <typename T>
struct Vector<T, 2>
{
    union {
        struct { T x, y; };
        T values[2]{};
    };

    static Vector const Zero;
    static Vector const One;
    static Vector const Up;
    static Vector const Left;
    static Vector const Down;
    static Vector const Right;

    Vector() = default;
    explicit Vector(T x, T y);
    Vector(T const (&& values)[2]);
    Vector(Vector const& other);

    Vector& operator=(Vector const& other);

    [[nodiscard]] T Norm() const;
    [[nodiscard]] T NormSquared() const;
    [[nodiscard]] T Dot(Vector const& other) const;
    [[nodiscard]] T DistanceTo(Vector const& other) const;
    [[nodiscard]] T DistanceToSquared(Vector const& other) const;

    [[nodiscard]] Vector Normalize() const;
    [[nodiscard]] Vector Clamp(Vector const& min, Vector const& max) const;
    [[nodiscard]] Vector Clamp(T min, T max) const;

    Vector& SelfNormalize();
    Vector& SelfClamp(Vector const& min, Vector const& max);
    Vector& SelfClamp(T min, T max);

    [[nodiscard]] Vector operator+(Vector const& other) const;
    [[nodiscard]] Vector operator-(Vector const& other) const;
    [[nodiscard]] Vector operator*(Vector const& other) const;
    [[nodiscard]] Vector operator/(Vector const& other) const;
    [[nodiscard]] Vector operator*(T scalar) const;
    [[nodiscard]] Vector operator/(T scalar) const;
    [[nodiscard]] Vector operator-() const;

    Vector& operator+=(Vector const& other);
    Vector& operator-=(Vector const& other);
    Vector& operator*=(Vector const& other);
    Vector& operator/=(Vector const& other);
    Vector& operator*=(T scalar);
    Vector& operator/=(T scalar);

    [[nodiscard]] explicit operator bool() const;
    [[nodiscard]] bool operator==(Vector const& other) const;
    [[nodiscard]] bool operator!=(Vector const& other) const;
    [[nodiscard]] bool operator<=(Vector const& other) const;
    [[nodiscard]] bool operator>=(Vector const& other) const;
};

#include "Vector2.inl"

#endif // !VECTOR2__H


