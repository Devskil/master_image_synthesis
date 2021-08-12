#pragma once

#include "vec.h"
#include "mat.h"

class Camera
{
public:
    Camera() = default;
    void lookAt(const Point& _target, const Point& _center, const Vector& _up);
    Transform view() const;
    Transform projection(const float width, const float height, const float fov) const;
protected:
    Point center;
    Point target;
    Vector up;
    float radius = 40;
};

