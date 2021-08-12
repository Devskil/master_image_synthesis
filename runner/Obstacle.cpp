#include "Obstacle.h"
#include "Tube.h"
#include "program.h"
#include "texture.h"
#include "uniforms.h"
#include "window.h"
#include "wavefront.h"

Obstacle::Obstacle(const Point& position, const float width, const float depth, const int index) :depth(depth), width(width),
                                                                                            position(position), hitbox(width, depth), index(index),
                                                                                            mesh(GL_TRIANGLE_STRIP) {}

void Obstacle::init()
{
    hitbox.getCenter() = position;
    program = read_program("data/shaders/cube.glsl");
    //program = read_program("data/shaders/mesh_color.glsl");
    program_print_errors(program);
    mesh = read_mesh(smart_path("data/cube.obj"));
    texture = read_texture(0, "data/Tex_Grid_DarkBlue.png");
}

void Obstacle::render(const Camera& camera, const Tube& tube)
{
    glUseProgram(program);
    Transform m = Identity()
        * translateToTube(tube)
        * rotateToTube(tube)
        * Scale(0.001 * width, 0.03, 0.03 * depth);
    Transform v = camera.view();
    Transform p = camera.projection(window_width(), window_height(), 45);
    Transform mvp = p * v * m;
    program_uniform(program, "mesh_color", Color(0, 0, 0, 1));
    program_uniform(program, "mvpMatrix", mvp);
    program_use_texture(program, "texture0", 0, texture);
    mesh.draw(program, true, true, false, false, false);
}

void Obstacle::quit()
{
    mesh.release();
    release_program(program);
    glDeleteTextures(1, &texture);
}

//Gives the centered around 0 0 0 cube's first point
Point Obstacle::defaultPosition() {
    return Point(position);
}

Transform Obstacle::translateToTube(const Tube& tube) {
    Point pos(defaultPosition());
    pos.z -= 0.2f;
    Vector v(tube.getWorldPosition(pos.x, pos.y, pos.z));
    return Translation(v);
}

Transform Obstacle::rotateToTube(const Tube& tube) {
    Point pos(defaultPosition());
    return tube.getWorldRotation(pos.x, pos.y);
}