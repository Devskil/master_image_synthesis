#include "app.h"
#include "mesh.h"
#include "Tube.h"
#include "Player.h"
#include "Camera.h"
#include "draw.h"

class Runner : public App {
    public:
        Runner() : App(1024, 640) {}
        int init() {
            tube.init();
            player.init();

            glClearColor(0.25f, 0.88f, 0.82f, 1.f);
            glClearDepth(1.f);
            glDepthFunc(GL_LESS);
            glEnable(GL_DEPTH_TEST);

            return 0;
        }

        int quit()
        {
            player.quit();
            tube.quit();
            return 0;
        }

        int update(const float time, const float delta)
        {
            tube.update(player.getPosition().x);
            player.update(delta);
            checkCollisions();

            if (!player.getAlive()) {
                collided = false;
                player.restart();
                tube.quit();
                float r = (float)(rand()) / (float)(RAND_MAX);
                float g = (float)(rand()) / (float)(RAND_MAX);
                float b = (float)(rand()) / (float)(RAND_MAX);
                glClearColor(r, g, b, 1.f);
                tube.init();
            }
            Point playerPosition(player.getPosition());
            player.getWorldPosition() = tube.getWorldPosition(playerPosition.x, playerPosition.y, playerPosition.z);
            player.getWorldRotation() = tube.getWorldRotation(playerPosition.x, playerPosition.y);
            Vector forward = tube.getPositionForward(player.getPosition().x);
            Point targetView = tube.getWorldPosition(playerPosition.x + 5.f, playerPosition.y, playerPosition.z);    
            Vector up = tube.getPositionUp(player.getPosition().x, player.getPosition().y, forward);
            Point cameraPosition = player.getWorldPosition() + up * 0.5 - forward * 0.8;
            Vector upVector = normalize(up * 0.5 + forward * 0.8);
            camera.lookAt(targetView, cameraPosition, upVector);

            

            return 0;
        }

        int render() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            tube.render(player.getPosition().y, camera);
            player.render(player.getWorldPosition(), player.getWorldRotation(), camera);

            return 1;
        }

        void checkCollisions()
        {
            std::deque<Obstacle> obstacles = tube.getObstacles();
            for (const auto& obstacle : obstacles)
            {
                if (Hitbox::collision(player.getHitbox(), obstacle.getHitbox())) {
                    player.die();
                    break;
                }
            }
        }

        protected:
            bool collided = false;
            Camera camera;
            Tube tube;
            Player player;
            
};

int main(int argc, char* args[])
{
    srand(time(NULL));
    
    Runner app;
    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << "std::exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Unhandled exception." << std::endl;
        return -1;
    }
    return 0;
}