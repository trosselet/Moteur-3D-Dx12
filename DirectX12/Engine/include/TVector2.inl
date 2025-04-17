#include "TVector2.h"

namespace Engine
{
    template<typename T>
    inline Vector2<T> Vector2<T>::operator+(const Vector2& other) const
    {
        return Vector2<T>(x + other.x, y + other.y);
    }

    template<typename T>
    inline Vector2<T> Vector2<T>::operator-(const Vector2& other) const
    {
        return Vector2<T>(x - other.x, y - other.y);
    }

    template<typename T>
    inline Vector2<T> Vector2<T>::operator*(T number) const
    {
        return Vector2<T>(x * number, y * number);
    }

    template<typename T>
    inline Vector2<T> Vector2<T>::operator/(T number) const
    {
        return Vector2<T>(x / number, y / number);
    }

    template<typename T>
    inline Vector2<T>& Vector2<T>::operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    template<typename T>
    inline Vector2<T>& Vector2<T>::operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template<typename T>
    inline Vector2<T>& Vector2<T>::operator*=(T number)
    {
        x *= number;
        y *= number;
        return *this;
    }

    template<typename T>
    inline Vector2<T>& Vector2<T>::operator/=(T number)
    {
        x /= number;
        y /= number;
        return *this;
    }
    template<typename T>
    inline bool Vector2<T>::Normalize()
    {
        T lengthSquared = x * x + y * y;

        if (lengthSquared > 0)
        {
            float xhalf = 0.5f * lengthSquared;
            int i = *(int*)&lengthSquared;
            i = 0x5f3759df - (i >> 1);
            float invSqrt = *(float*)&i;
            invSqrt = invSqrt * (1.5f - xhalf * invSqrt * invSqrt);

            x *= invSqrt;
            y *= invSqrt;

            return true;
        }

        return false;
    }
}