#include "Tube.h"
#include "draw.h"
#include "program.h"
#include "texture.h"
#include "uniforms.h"

Tube::Tube() {};

void Tube::init() {
    tube = GL_TRIANGLE_STRIP;
    forwardPosition = 0;
    index = 0;

    program = read_program("data/shaders/mesh_color.glsl");
    program_print_errors(program);
    //texture = read_texture(0, "data/Tex_Grid_Red.png");

    initPoints(); //TP 2 part 1 + part 2
    maxIndex = (nbChaikinPoints - 2) * nbCirclePoints * 4;
    firstOrthogonal(); //TP 2 part 3
    circles(); //TP 2 part 4
    generateMesh(); //TP 2 part 5

    for (unsigned i = 3; i < points.size() - 2; ++i) {
        generateObstacle(forwardPosition + forwardStep * i, i);
    }

}

void Tube::initPoints() {
    points.push_back(Point(0.0, 0.0, 0.0));
    points.push_back(Point(0.0, 0.0, 1.0));
    lastDir = Vector(points.back());
    float x, y, z;
    for (unsigned i = 2; i < nbPoints; ++i) {
        x = (float)(rand() % 100 + 1);
        y = (float)(rand() % 100 + 1);
        z = (float)(rand() % 100 + 1);
        auto vec = Vector(x, y, z);
        points.push_back((points[i - 1] + normalize(vec)));
    }
    nbChaikinPoints = points.size();
    for (int i = 0; i < interpolationCount; ++i) {
        nbChaikinPoints = (nbChaikinPoints - 1) * 2;
    }
    chaikinsPoints.resize(nbChaikinPoints);
    subdivideChainkin();
}

void Tube::subdivideChainkin() {
    for (int i = 0; i < points.size(); ++i) {
        chaikinsPoints[i] = points[i];
    }
    std::vector<Point> tmp;
    tmp.resize(nbChaikinPoints);
    unsigned currentSize = nbPoints;
    for (int j = 1; j <= interpolationCount; ++j) {
        unsigned index = 0;
        for (int i = 1; i < currentSize; ++i) {
            tmp[index++] = (chaikinsPoints[i - 1] + (chaikinsPoints[i] - chaikinsPoints[i - 1]) / 4.f);
            tmp[index++] = (chaikinsPoints[i - 1] + (chaikinsPoints[i] - chaikinsPoints[i - 1]) * 3.f / 4);
        }
        chaikinsPoints.swap(tmp);
        currentSize = (currentSize - 1) * 2;
    }

}

void Tube::firstOrthogonal() {
    Vector dir = chaikinsPoints[1] - chaikinsPoints[0];
    lastOrthogonal = cross(dir, Vector(1, 0, 0));
    if (length(lastOrthogonal) == 0.0f) {
        lastOrthogonal = cross(dir, Vector(0, 0, 1));
    }
    lastOrthogonal = normalize(lastOrthogonal) * 0.1;
    orthogonalsTab.push_back(lastOrthogonal);
}

//compute the circles
void Tube::circles() {
    std::vector<Point> circle;
    circle.push_back(chaikinsPoints[0] + lastOrthogonal);
    circlesTab.push_back(circle);


    //compute the first orthogonal

    for (std::size_t i = 2; i < chaikinsPoints.size(); ++i) {
        Transform R = Rotation(chaikinsPoints[i - 1] - chaikinsPoints[i - 2], chaikinsPoints[i] - chaikinsPoints[i - 1]);
        lastOrthogonal = normalize(R(lastOrthogonal)) * 0.1;
        orthogonalsTab.push_back(lastOrthogonal);
        std::vector<Point> tempCircle;
        tempCircle.push_back(chaikinsPoints[i - 1] + lastOrthogonal);
        circlesTab.push_back(tempCircle);
    }

    //rotate the orthogonals
    for (unsigned i = 0; i < chaikinsPoints.size() - 1; ++i) {
        for (unsigned j = 1; j < nbCirclePoints; ++j)
        {
            Vector v = Rotation(chaikinsPoints[i + 1] - chaikinsPoints[i], j * angle)(circlesTab[i][0] - chaikinsPoints[i]);
            circlesTab[i].push_back(chaikinsPoints[i] + v);
        }
    }
}

void Tube::generateMesh()
{
    for (unsigned i = 0; i < chaikinsPoints.size() - 2; ++i) {
        for (unsigned j = 0; j < nbCirclePoints; ++j)
        {
            auto nextJ = (j + 1) % nbCirclePoints;

            tube.normal(normalize(circlesTab[i + 1][j] - chaikinsPoints[i + 1]));
            tube.vertex(circlesTab[i + 1][j]);

            tube.normal(normalize(circlesTab[i][j] - chaikinsPoints[i]));
            tube.vertex(circlesTab[i][j]);

            tube.normal(normalize(circlesTab[i + 1][nextJ] - chaikinsPoints[i + 1]));
            tube.vertex(circlesTab[i + 1][nextJ]);

            tube.normal(normalize(circlesTab[i][nextJ] - chaikinsPoints[i]));
            tube.vertex(circlesTab[i][nextJ]);

            tube.restart_strip();
        }
    }
}

void Tube::updateMesh()
{
    for (unsigned i = 0; i < forwardStep; ++i) {
        unsigned k = nbChaikinPoints - forwardStep + i - 2;
        for (unsigned j = 0; j < nbCirclePoints; ++j)
        {
            auto nextJ = (j + 1) % nbCirclePoints;

            tube.normal(index, normalize(circlesTab[i + 1][j] - chaikinsPoints[i + 1]));
            tube.vertex(index, circlesTab[i + 1][j]);
            ++index;
            tube.normal(index, normalize(circlesTab[i][j] - chaikinsPoints[i]));
            tube.vertex(index, circlesTab[i][j]);
            ++index;
            tube.normal(index, normalize(circlesTab[i + 1][nextJ] - chaikinsPoints[i + 1]));
            tube.vertex(index, circlesTab[i + 1][nextJ]);
            ++index;
            tube.normal(index, normalize(circlesTab[i][nextJ] - chaikinsPoints[i]));
            tube.vertex(index, circlesTab[i][nextJ]);
            ++index;
            tube.restart_strip();
            index = index % maxIndex;

        }
    }
}

void Tube::update(float playerPosition)
{
    if (playerPosition - forwardPosition > forwardStep * 2.0f)
    {
        generateNewPoint();
    }
}

//To avoid memory leak, we have to delete the oldest part of the tube when adding a new one.
//Then generate the new point's circle.
//And lastly, regen the mesh from the updated circle vector.
//This function basically does the same job as the init but only on the last point of the curve, instead of all.
void Tube::generateNewPoint()
{
    //erase the first point of the cube to leave space for a new one
    float x, y, z;
    for (unsigned i = 0; i < nbPoints - 1; ++i)
    {
        points[i] = points[i + 1];
    }
    x = (float)(rand() % 100 + 1);
    y = (float)(rand() % 100 + 1);
    z = (float)(rand() % 100 + 1);
    auto vec = Vector(x, y, z);
    points[nbPoints - 1] = (points[nbPoints - 2] + normalize(vec));

    subdivideChainkin();
    forwardPosition += forwardStep;

    //Same as above, but with the orthogonals
    for (int i = 0; i < nbChaikinPoints - 1 - forwardStep; ++i)
    {
        orthogonalsTab[i] = orthogonalsTab[i + forwardStep];
    }

    //Compute the new last orthogonal and adds it to the end of the circles array
    std::vector<Point> newOrthogonal;
    newOrthogonal.resize(nbCirclePoints);
    newOrthogonal[0] = chaikinsPoints[nbChaikinPoints - 1 - forwardStep - 1] + lastOrthogonal;
    circlesTab[0] = newOrthogonal;


    //Rotate the orthogonals of the new circle
    for (int i = 0; i < forwardStep; ++i)
    {
        unsigned j = nbChaikinPoints - forwardStep + i;
        Transform R = Rotation(chaikinsPoints[j - 1] - chaikinsPoints[j - 2], chaikinsPoints[j] - chaikinsPoints[j - 1]);
        lastOrthogonal = normalize(R(lastOrthogonal)) * 0.1;
        orthogonalsTab[j - 1] = lastOrthogonal;

        std::vector<Point> newCircle;
        newCircle.resize(nbCirclePoints);
        newCircle[0] = chaikinsPoints[j - 1] + lastOrthogonal;
        circlesTab[i + 1] = newCircle;
    }

    //Rotate the orthogonalsTab to circles
    for (unsigned i = 0; i < forwardStep + 1; ++i)
    {
        for (unsigned j = 1; j < nbCirclePoints; ++j)
        {
            unsigned k = nbChaikinPoints - 1 - forwardStep + i - 1; //all points index
            Vector trans = Rotation(chaikinsPoints[k + 1] - chaikinsPoints[k], j * angle)(circlesTab[i][0] - chaikinsPoints[k]);
            circlesTab[i][j] = chaikinsPoints[k] + trans;
        }
    }

    //Delete the mesh of the first point of the curve, and adds the mesh of the newly calculated circle
    updateMesh();

    //Gets rid of obstacles behind the player
    for (auto it = obstaclesTab.begin(); it != obstaclesTab.end(); )
    {
        (*it).getIndex() -= 1;
        if ((*it).getIndex() <= 0) {
            std::cout << "erase" << std::endl;
            (*it).quit();
            it = obstaclesTab.erase(it);
        }
        else {
            ++it;
        }

    }
    //Generate a new obstacle at the end of the currently generated track
    generateObstacle(nbChaikinPoints - forwardStep - 1 + forwardPosition, nbPoints - 2);

}

void Tube::generateObstacle(const int startPos, const int index)
{
    float x = startPos + (float)(rand()) / (float)(RAND_MAX / (forwardStep - 3));
    float y = (float)(rand()) / (float)(RAND_MAX / (360.0f));
    float z = 1.5f;
    int width = 3 + rand() % 6;
    Obstacle o(Point(x, y, z), angle * width, 0.5, index);
    o.init();
    obstaclesTab.push_back(o);
}

void Tube::render(const float posY, const Camera& camera)
{
    glUseProgram(program);
    Transform m = Identity();
    Transform v = camera.view();
    Transform p = camera.projection(window_width(), window_height(), 45);
    program_uniform(program, "mesh_color", Color(1, 1, 1, 1));
    program_uniform(program, "mvpMatrix", p * v * m);
    program_use_texture(program, "texture0", 0, texture);
    tube.draw(program, true, false, false, false, false);

    Point worldPos;
    Transform rotation;
    int i = 0;
    for (auto& obstacle : obstaclesTab)
    {
        i++;
        obstacle.render(camera, *this);
    }
}

void Tube::quit()
{
    tube.release();
    release_program(program);
    glDeleteTextures(1, &texture);
    for (auto& obstacle : obstaclesTab)
    {
        obstacle.quit();
    }
    obstaclesTab.clear();
    points.clear();
    chaikinsPoints.clear();
    orthogonalsTab.clear();
    circlesTab.clear();

}

//Returns the position on the tube for the given pos.
Point Tube::getWorldPosition(float pos, float rotation, float height) const
{
    float i = pos - forwardPosition;

    Point p1 = chaikinsPoints[int(i)];
    Point p2 = chaikinsPoints[int(i) + 1];
    float coeff = i - int(i);
    Point realPos = p1 + coeff * (p2 - p1);

    return realPos + Rotation(p2 - p1, rotation)(orthogonalsTab[int(i)]) * height;
}
//Returns the rotation on the tube for the given pos and local rotation.
Transform Tube::getWorldRotation(float pos, float rotation) const
{
    Vector vz = getPositionForward(pos);
    Vector vy = getPositionUp(pos, rotation, vz);
    Vector vx = -normalize(cross(vz, vy));

    Transform R =Transform(vx.x, vy.x, vz.x, 0,
                            vx.y, vy.y, vz.y, 0,
                            vx.z, vy.z, vz.z, 0,
                            0, 0, 0, 1);

    return R;
}

Vector Tube::getPositionForward(float pos) const
{
    int i = pos - forwardPosition;
    return normalize(chaikinsPoints[i + 1] - chaikinsPoints[i]);
}

Vector Tube::getPositionUp(float pos, float rotation, Vector forward) const
{
    int i = pos - forwardPosition;
    return normalize(Rotation(forward, rotation)(orthogonalsTab[i]));
}

Mesh& Tube::getMesh()
{
    return tube;
}

const std::deque<Obstacle>& Tube::getObstacles() const
{
    return obstaclesTab;
}
