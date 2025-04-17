#ifndef TVECTOR2__H
#define TVECTOR2__H

namespace Engine
{
    template <typename T>
    class Vector2 {
    public:
        T x;
        T y;

        Vector2() : x(0), y(0) {}
        Vector2(T x, T y) : x(x), y(y) {}

        Vector2 operator+(const Vector2& other) const;

        Vector2 operator-(const Vector2& other) const;

        Vector2 operator*(T number) const;

        Vector2 operator/(T number) const;

        Vector2& operator+=(const Vector2& other);

        Vector2& operator-=(const Vector2& other);

        Vector2& operator*=(T number);

        Vector2& operator/=(T number);

        bool Normalize();

    };

}

#include "TVector2.inl"

#endif // !TVECTOR2__H


