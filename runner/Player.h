#pragma once

#include "mesh.h"
#include "Camera.h"
#include "Hitbox.h"
class Player
{

public:
    Player();

    void init();
    void update(const float delta);
    void render(const Point& worldPosition, const Transform& rotation, const Camera& camera);
    void quit();

    void die();
    void restart();

    Mesh& getMesh() { return mesh; }
    const Point& getPosition() { return position; }
    Point& getWorldPosition() { return worldPosition; }
    Transform& getWorldRotation() { return worldRotation; }
    const Hitbox& getHitbox() { return hitbox; }
    bool getAlive() const { return alive; }
protected:
    Mesh mesh;
    GLuint program;
    GLuint texture;

    const float MAX_CUBE_ROTATION = 25.0f;
    const float TIME_TO_MAX_SPEED = 10000.0f;

    Point position;
    Point worldPosition;
    Transform worldRotation;
    Hitbox hitbox;

    bool alive;

    //CONTROLS
    float forwardSpeed;
    float rotationSpeed;
    float acceleration;
    float cubeLocalRotation;

    void moveForward(const float delta);
    void turnControl(const float delta);
    void turn(const float delta);
};

