#pragma once

#include "mesh.h"
#include "Camera.h"

class Hitbox
{
public:
    Hitbox() = delete;
    Hitbox(float width, float depth);
    Point& getCenter() { return center; }
    float getWidth() const { return width; }
    float getDepth() const { return depth; }
    static bool collision(const Hitbox& player, const Hitbox& obstacle);
protected:
    Point center;
    float width;
    float depth;

    static float min(float a, float b);
};
