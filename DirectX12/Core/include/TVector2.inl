#ifndef VECTOR2__INL
#define VECTOR2__INL

template <typename T>
Vector<T, 2> const Vector<T, 2>::Zero = Vector(0, 0);

template <typename T>
Vector<T, 2> const Vector<T, 2>::One = Vector(1, 1);

template <typename T>
Vector<T, 2> const Vector<T, 2>::Up = Vector(0, 1);

template <typename T>
Vector<T, 2> const Vector<T, 2>::Left = Vector(-1, 0);

template <typename T>
Vector<T, 2> const Vector<T, 2>::Down = Vector(0, -1);

template <typename T>
Vector<T, 2> const Vector<T, 2>::Right = Vector(1, 0);


template <typename T>
Vector<T, 2>::Vector(T const x, T const y) :
    x(x),
    y(y) {
}

template <typename T>
Vector<T, 2>::Vector(T const (&& values)[2]) :
    x(values[0]),
    y(values[1]) {
}

template <typename T>
Vector<T, 2>::Vector(Vector const& other) :
    x(other.x),
    y(other.y) {
}


template <typename T>
Vector<T, 2>& Vector<T, 2>::operator=(Vector<T, 2> const& other)
{
    if (this == &other) return *this;
    x = other.x;
    y = other.y;
    return *this;
}


template <typename T>
T Vector<T, 2>::Norm() const { return std::sqrt(x * x + y * y); }

template <typename T>
T Vector<T, 2>::NormSquared() const { return x * x + y * y; }

template <typename T>
T Vector<T, 2>::Dot(Vector<T, 2> const& other) const { return x * other.x + y * other.y; }

template <typename T>
T Vector<T, 2>::DistanceTo(Vector const& other) const { return (other - *this).Norm(); }

template <typename T>
T Vector<T, 2>::DistanceToSquared(Vector const& other) const { return (other - *this).NormSquared(); }


template <typename T>
Vector<T, 2> Vector<T, 2>::Normalize() const
{
    T const norm = Norm();
    assert(norm != 0);
    T const invNorm = 1 / norm;
    return Vector(x * invNorm, y * invNorm);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::Clamp(Vector const& min, Vector const& max) const
{
    assert(min.x <= max.x && min.y <= max.y);
    return Vector(
        (x < min.x) ? min.x : ((x > max.x) ? max.x : x),
        (y < min.y) ? min.y : ((y > max.y) ? max.y : y)
    );
}

template <typename T>
Vector<T, 2> Vector<T, 2>::Clamp(T const min, T const max) const
{
    assert(min <= max);
    return Vector(
        (x < min) ? min : ((x > max) ? max : x),
        (y < min) ? min : ((y > max) ? max : y)
    );
}


template <typename T>
Vector<T, 2>& Vector<T, 2>::SelfNormalize()
{
    T const norm = Norm();
    assert(norm != 0);
    T const invNorm = 1 / norm;
    x *= invNorm;
    y *= invNorm;
    return *this;
}

template <typename T>
Vector<T, 2>& Vector<T, 2>::SelfClamp(Vector const& min, Vector const& max)
{
    assert(min.x <= max.x && min.y <= max.y);

    if (x < min.x) x = min.x;
    else if (x > max.x) x = max.x;

    if (y < min.y) y = min.y;
    else if (y > max.y) y = max.y;

    return *this;
}

template <typename T>
Vector<T, 2>& Vector<T, 2>::SelfClamp(T const min, T const max)
{
    assert(min <= max);

    if (x < min) x = min;
    else if (x > max) x = max;

    if (y < min) y = min;
    else if (y > max) y = max;

    return *this;
}


template <typename T>
Vector<T, 2> Vector<T, 2>::operator+(Vector const& other) const { return Vector(x + other.x, y + other.y); }

template <typename T>
Vector<T, 2> Vector<T, 2>::operator-(Vector const& other) const { return Vector(x - other.x, y - other.y); }

template <typename T>
Vector<T, 2> Vector<T, 2>::operator*(Vector const& other) const { return Vector(x * other.x, y * other.y); }

template <typename T>
Vector<T, 2> Vector<T, 2>::operator/(Vector const& other) const
{
    assert(other.x != 0 && other.y != 0);
    return Vector(x / other.x, y / other.y);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::operator*(T const scalar) const { return Vector(x * scalar, y * scalar); }

template <typename T>
Vector<T, 2> Vector<T, 2>::operator/(T const scalar) const
{
    assert(scalar != 0);
    T const invScalar = 1 / scalar;
    return Vector(x * invScalar, y * invScalar);
}

template <typename T>
Vector<T, 2> Vector<T, 2>::operator-() const { return Vector(-x, -y); }


template <typename T>
Vector<T, 2>& Vector<T, 2>::operator+=(Vector const& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

template <typename T>
Vector<T, 2>& Vector<T, 2>::operator-=(Vector const& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

template <typename T>
Vector<T, 2>& Vector<T, 2>::operator*=(Vector const& other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}

template <typename T>
Vector<T, 2>& Vector<T, 2>::operator/=(Vector const& other)
{
    assert(other.x != 0 && other.y != 0);
    x /= other.x;
    y /= other.y;
    return *this;
}

template <typename T>
Vector<T, 2>& Vector<T, 2>::operator*=(T const scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

template <typename T>
Vector<T, 2>& Vector<T, 2>::operator/=(T const scalar)
{
    assert(scalar != 0);
    T const invScalar = 1 / scalar;
    x *= invScalar;
    y *= invScalar;
    return *this;
}


template <typename T>
Vector<T, 2>::operator bool() const { return x != 0 || y != 0; }

template <typename T>
bool Vector<T, 2>::operator==(Vector const& other) const { return this == &other || (x == other.x && y == other.y); }

template <typename T>
bool Vector<T, 2>::operator!=(Vector const& other) const { return this != &other && (x != other.x || y != other.y); }

template <typename T>
bool Vector<T, 2>::operator<=(Vector const& other) const { return this == &other || (x <= other.x && y <= other.y); }

template <typename T>
bool Vector<T, 2>::operator>=(Vector const& other) const { return this == &other || (x >= other.x && y >= other.y); }

#endif //!VECTOR2__INL