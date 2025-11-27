#ifndef STRUCTVECTOR_H
#define STRUCTVECTOR_H
#include<cmath>
struct Vector2
{
    float x, y;
    Vector2(float x = 0, float y = 0) :x(x), y(y) {}
    float distanceTo(const Vector2& other)const {
        float dx = other.x - x;float dy = other.y - y;
        return std::sqrt(dx * dx + dy * dy);
    }
    Vector2 normalized()const {
        float len = std::sqrt(x * x + y * y);
        return len > 0 ? Vector2(x / len, y / len) : *this;
    }
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other)const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar)const { return Vector2(x * scalar, y * scalar); }
};

#endif // STRUCTVECTOR_H
