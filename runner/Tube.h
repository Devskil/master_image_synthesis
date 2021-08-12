#pragma once

#include "mesh.h"
#include "Camera.h"
#include "Obstacle.h"
#include <queue>
#include <cmath>

class Tube
{
public:
    Tube();
    void init();
    void render(const float posY, const Camera& camera);
    void quit();
    void update(float playerPosition);

    void initPoints();
    void subdivideChainkin();
    void circles();
    void firstOrthogonal();
    void generateMesh();
    void updateMesh();

    void generateNewPoint();
    void generateObstacle(const int startPos, const int index);

    Point getWorldPosition(float pos, float rotation, float height) const;
    Transform getWorldRotation(float pos, float rotation) const;
    Vector getPositionForward(float pos) const;
    Vector getPositionUp(float pos, float rotation, Vector forward) const;

    Mesh& getMesh();
    const std::deque<Obstacle>& getObstacles() const;

protected:
    Mesh tube;
    GLuint program;
    GLuint texture;

    static const unsigned nbPoints = 25; // must be 5 or more for the interpolation to work properly
    static const unsigned nbCirclePoints = 50; // define the circle's roundness (more = more round), must be at least 10 (or some obstacles won't be dodgeable)
    static const unsigned angle = 360 / nbCirclePoints;
    static const unsigned interpolationCount = 5;
    int forwardStep = std::ldexp(1.0, interpolationCount);  //Number of steps between two point of the main curve.

    std::vector<Point> points;
    std::vector<Point> chaikinsPoints;
    std::vector<Vector> orthogonalsTab;
    std::vector<std::vector<Point>> circlesTab;
    Vector lastOrthogonal;
    Vector lastDir;
    int forwardPosition;
    int index;
    int maxIndex;
    unsigned nbChaikinPoints;

    std::deque<Obstacle> obstaclesTab;
};

