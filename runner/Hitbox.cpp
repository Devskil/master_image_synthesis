#include "Hitbox.h"
#include "window.h"

Hitbox::Hitbox(float width, float depth) : width(width), depth(depth) {}

bool Hitbox::collision(const Hitbox& player, const Hitbox& obstacle) {
    //Test collision in depth
    if (player.center.x < obstacle.center.x + obstacle.depth &&
        player.center.x + player.depth > obstacle.center.x) {
        //Test collision in width (no need to test height because all objects are attached to the tube)
        if (player.center.y < obstacle.center.y) {
            if (min(obstacle.center.y - player.center.y, player.center.y - obstacle.center.y + 360) <= player.width / 2.0f + obstacle.width / 2.0f)
                return true;
        }
        else {
            if (min(player.center.y - obstacle.center.y, obstacle.center.y - (player.center.y - 360)) <= player.width / 2.0f + obstacle.width / 2.0f)
                return true;
        }
    }
    return false;

}

float Hitbox::min(float a, float b) {
    if (a > b)
        return b;
    else
        return a;
}