#include "Player.h"
#include "wavefront.h"
#include "program.h"
#include "texture.h"
#include "uniforms.h"
#include "window.h"

Player::Player() : position(Point(0, 0, 1.5f)), hitbox(15, 1.5f), acceleration(0.1f), forwardSpeed(0.065f),
                    rotationSpeed(0.25f), cubeLocalRotation(0), alive(true) {}

void Player::init() {
    program = read_program("data/shaders/cube.glsl");
    program_print_errors(program);
    mesh = read_mesh("data/cube.obj");
    texture = read_texture(0, "data/player_text.png");

}

void Player::update(const float delta) {
    if (!alive) return;
    if (acceleration < 1.0f) {
        acceleration += delta / TIME_TO_MAX_SPEED;
        if (acceleration > 1.0f) 
            acceleration = 1.0f;
    }

    moveForward(delta);
    turnControl(delta);

    hitbox.getCenter() = position;
}

void Player::render(const Point& worldPosition, const Transform& rotation, const Camera& camera) {

    glUseProgram(program);
    Transform m = Translation(worldPosition.x, worldPosition.y, worldPosition.z)
        * rotation //Rotate cube
        * RotationZ(cubeLocalRotation) // Localy rotate to skew the cube in the last moved direction
        * Scale(0.01, 0.01, 0.01);
    Transform v = camera.view();
    Transform p = camera.projection(window_width(), window_height(), 45);
    program_uniform(program, "mvpMatrix", p * v * m);
    program_use_texture(program, "texture0", 0, texture);
    mesh.draw(program, true, true, false, true, false);
}

void Player::quit() {
    mesh.release();
    release_program(program);
}

void Player::die() {
    acceleration = 0;
    alive = false;
}

void Player::moveForward(const float delta) {
    position.x = position.x + forwardSpeed * acceleration * delta;
}

void Player::turnControl(const float delta) {
    bool leftKey = key_state('q');
    bool rightKey = key_state('d');
    if ((!leftKey && !rightKey) || (leftKey && rightKey)) {
        if (cubeLocalRotation > 0) {
            cubeLocalRotation -= rotationSpeed * delta;
            if (cubeLocalRotation < 0) cubeLocalRotation = 0;
        }
        else if (cubeLocalRotation < 0) {
            cubeLocalRotation += rotationSpeed * delta;
            if (cubeLocalRotation > 0) cubeLocalRotation = 0;
        }
    }
    else if (leftKey) {
        turn(-delta);
    }
    else if (rightKey) {
        turn(delta);
    }
}

void Player::turn(const float delta)
{
    position.y = position.y + rotationSpeed * acceleration * delta;
    while (position.y >= 360.0f) 
        position.y -= 360.0f;
    while (position.y < 0.0f) 
        position.y += 360.0f;
    if (position.y == 360.0f) 
        position.y = 0;

    cubeLocalRotation += rotationSpeed * acceleration * delta;
    if (cubeLocalRotation > MAX_CUBE_ROTATION) 
        cubeLocalRotation = MAX_CUBE_ROTATION;
    else if 
        (cubeLocalRotation < -MAX_CUBE_ROTATION) cubeLocalRotation = -MAX_CUBE_ROTATION;
}
void Player::restart() {
    acceleration = 0.1f;
    alive = true;
    position = Point(0, 0, 1.5f);
}