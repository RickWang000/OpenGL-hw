#ifndef CAMERA_H
#define CAMERA_H


#include <QMatrix4x4>
#include <QVector3D>
#include <QMatrix3x3>

class Camera {
public:
    Camera() : eye(0, 0, -1), up(0, 1, 0), center(0, 0, 0), zoom(1.0) { set_initial_distance_ratio(1.0); }
    Camera(QVector3D a, QVector3D b, QVector3D c) : eye(a), up(b), center(c), zoom(1.0) { set_initial_distance_ratio(1.0); }
    Camera(QVector3D a, QVector3D b, QVector3D c, float dis) : eye(a), up(b), center(c), zoom(1.0) { set_initial_distance_ratio(dis); }
    ~Camera() {}

public:
    QVector3D eye;
    QVector3D up;
    QVector3D center;
    float zoom;
    float distance_r;

public:
    void set_initial_distance_ratio(float r);
    QMatrix4x4 get_camera_matrix();
    void translate_left(float dis);
    void translate_up(float dis);
    void rotate_left(float degree);
    void rotate_up(float degree);
    void zoom_near(float degree);
    void translate_forward(float dis);
};

QMatrix4x4 rotate_mat(const float degree, const QVector3D axis);


#endif // CAMERA_H
