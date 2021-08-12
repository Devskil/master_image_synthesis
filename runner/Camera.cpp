#include "Camera.h"

//Setup for view function below
void Camera::lookAt(const Point& _target, const Point& _center, const Vector& _up) {
    target = _target;
    up = _up;
    center = _center;
}

//https://www.3dgep.com/understanding-the-view-matrix/#Look_At_Camera version optimized look at with inversion of row/column (column/row major type of deal)
Transform Camera::view() const
{
    Vector zAxis = normalize(center - target);
    Vector xAxis = normalize(cross(up, zAxis));
    Vector yAxis = cross(zAxis, xAxis);

    return Transform(
        xAxis.x, xAxis.y, xAxis.z, -dot(xAxis, Vector(center)),
        yAxis.x, yAxis.y, yAxis.z, -dot(yAxis, Vector(center)),
        zAxis.x, zAxis.y, zAxis.z, -dot(zAxis, Vector(center)),
        0, 0, 0, 1
    );
}

//https://perso.univ-lyon1.fr/jean-claude.iehl/Public/educ/M1IMAGE/html/classOrbiter.html#af72f8177ec74237d2827aa2492cbec45 Copy + paste
Transform Camera::projection(const float width, const float height, const float fov) const
{
    // calcule la distance entre le centre de l'objet et la camera
    Transform t = view();
    Point c = t(center);
    float d = -c.z;

    // regle near et far en fonction du centre et du rayon englobant l'objet 
    return Perspective(fov, width / height, std::max(0.1f, d - radius), std::max(1.f, d + radius));
}
