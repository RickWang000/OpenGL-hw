#include "Camera.h"
#include <QtCore/qmath.h>
#include <QTransform>
#define pi 3.1415926

QMatrix4x4 rotate_mat(const float degree, const QVector3D axis) {
    /*
    double theta = -degree * pi / 180.0;
    QMatrix4x4 R1;

    float R_two_data[] = { axis.x() * axis.x(), axis.x() * axis.y(), axis.x() * axis.z(), 0,
        axis.x() * axis.y(), axis.y() * axis.y(), axis.y() * axis.z(), 0,
        axis.x() * axis.z(), axis.y() * axis.z(), axis.z() * axis.z(), 0,
        0,0,0,1	};
    QMatrix4x4 R2(R_two_data);

    float R_three_data[] = { 0, axis.z(), -axis.y(), 0,
        -axis.z(), 0, axis.x(), 0,
        axis.y(), -axis.x(), 0, 0,
        0,0,0,1};
    QMatrix4x4 R3(R_three_data);

    QMatrix4x4 mat = R1 * (float)cos(theta) + (1 - (float)cos(theta)) * R2 + (float)sin(theta) * R3;
    */

    QMatrix4x4 mat;
    mat.rotate(degree, axis);
    return mat;
}

QMatrix4x4 Camera::get_camera_matrix() {
    QVector3D x_eye = (eye - center).normalized() * this->zoom * this->distance_r + center;

    QMatrix4x4 mat;
    mat.lookAt(x_eye, this->center, this->up);

    return mat;
}

void Camera::set_initial_distance_ratio(float r) {
    distance_r = r;
}

void Camera::translate_up(float dis) {
    QVector3D up_n = up.normalized();
    center += dis * up_n;
    eye += dis * up_n;
}

void Camera::translate_left(float dis) {
    QVector3D z_axis = (center - eye).normalized();
    QVector3D x_axis = QVector3D::crossProduct(up, z_axis).normalized();
    center += dis * x_axis;
    eye += dis * x_axis;
}

void Camera::rotate_left(float degree) {
    eye = eye - center;
    eye = rotate_mat(degree, up) * eye;
    eye = eye + center;
}

void Camera::rotate_up(float degree) {
    eye = eye - center;

    QVector3D x = QVector3D::crossProduct(up, eye);
    x.normalize();
    eye = rotate_mat(degree, x) * eye;

    up = QVector3D::crossProduct(eye, x);
    up.normalize();

    eye = eye + center;
}

void Camera::translate_forward(float dis) {
    QVector3D u = (center - eye).normalized();

    eye = eye + u * dis;
    center = center + u * dis;
}

void Camera::zoom_near(float dis) {
    zoom -= dis;
    const float zoom_max = 2.0;
    const float zoom_min = 0.1;
    zoom = qMax(qMin(zoom, zoom_max), zoom_min);
}

