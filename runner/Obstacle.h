#pragma once

#include "Hitbox.h"
#include "Camera.h"

class Tube;

class Obstacle
{
public:
    Obstacle() = delete;
    Obstacle(const Point& position, const float width, const float depth, const int index);
    void init();
    void render(const Camera& camera, const Tube& tube);
    void quit();
    const Hitbox& getHitbox() const { return hitbox; }
    int& getIndex() { return index; }
    const Point& getPosition() { return position; }

protected:
    GLuint program;
    GLuint texture;
    Mesh mesh;

    Point position;
    Hitbox hitbox;

    float depth, width;
    int index;

    Point defaultPosition();
    Transform translateToTube(const Tube& tube);
    Transform rotateToTube(const Tube& tube);
};
