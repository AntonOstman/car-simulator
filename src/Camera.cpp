#define GLM_SWIZZLE
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate

#include "Camera.hpp"
#include "glm/geometric.hpp"

glm::mat4 lookAt(glm::vec3 up, glm::vec3 eye, glm::vec3 center)
{
    glm::vec3 forward = glm::normalize(eye - center);
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    glm::mat4 lookAt = glm::mat4(1.0);

    lookAt[0] = glm::vec4(right, 0);
    lookAt[1] = glm::vec4(up, 0);
    lookAt[2] = glm::vec4(forward, 0);
    lookAt[3] = glm::vec4(eye, 1.0);
    return lookAt;
}

Camera::Camera()
{
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 eye = glm::vec3(0, 0, 10);
    glm::vec3 lookTowards = glm::vec3(0, 0, 0);
    _rotation = glm::mat4(1.0);
    _translation = glm::mat4(1.0);

    _cur_angle = glm::vec2(0, 0);
    _view = lookAt(up, eye, lookTowards);
}

glm::mat4 Camera::getViewToWorld()
{
    return _view;
}

void Camera::moveForward(float speed)
{
    _view[3] -= speed * _view[2];
}

void Camera::moveBack(float speed)
{
    _view[3] += speed * _view[2];
}

void Camera::moveLeft(float speed)
{
    _view[3] -= speed * _view[0];
}

void Camera::moveRight(float speed)
{
    _view[3] += speed * _view[0];
}

void Camera::moveUp(float speed)
{
    _view[3] += speed * _view[1];
}

void Camera::moveDown(float speed)
{
    _view[3] -= speed * _view[1];
}

void Camera::rotateRelative(glm::vec2 angle)
{
    _cur_angle += angle;

    if (abs((int)_cur_angle.y) > 40.0)
    {
        _cur_angle.y -= angle.y;
    }
    glm::quat pitch = glm::angleAxis((float)glm::radians(_cur_angle.y), glm::normalize(glm::vec3(_view[0])));
    glm::quat yaw = glm::angleAxis((float)glm::radians(_cur_angle.x), glm::normalize(glm::vec3(0,1,0)));
    glm::quat rot = pitch * yaw;

    glm::mat4 cameraRot = glm::mat4(rot);
    _view[0] = cameraRot[0];
    _view[1] = cameraRot[1];
    _view[2] = cameraRot[2];
}

void Camera::translate(glm::vec3 translation)
{
    glm::mat4 trans = glm::mat4(1.0);
    trans[3] = glm::vec4(translation, 1.0);

    _view = trans * _view;
}

void Camera::transform(glm::mat4 transform)
{
    _view = transform * _view;
}
