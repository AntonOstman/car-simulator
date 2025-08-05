#define GLM_FORCE_SWIZZLE
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate

#include "Camera.hpp"
#include "glm/geometric.hpp"

glm::mat4 lookAt(glm::vec3 up, glm::vec3 eye, glm::vec3 center)
{
    glm::vec3 forward = glm::normalize(center - eye);
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    glm::vec3 up2 = glm::normalize(glm::cross(forward, right)); // Recompute up to make sure orthagonal
    glm::mat4 lookAt = glm::mat4(1.0);

    lookAt[0] = glm::vec4(right, 0);
    lookAt[1] = glm::vec4(up2, 0);
    lookAt[2] = glm::vec4(forward, 0);
    lookAt[3] = glm::vec4(eye, 1.0);
    return lookAt;
}

Camera::Camera()
{
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 eye = glm::vec3(0, 0, -10);
    glm::vec3 lookTowards = glm::vec3(0, 0, 0);

    _cur_angle = glm::vec2(0, 0);
    _view = lookAt(up, eye, lookTowards);
    // _view = glm::lookAt(eye, lookTowards, up);
}

/*
   Returns the view to world camera matrix
*/

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

    if (abs((int)_cur_angle.y) > 50.0)
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

void Camera::translateWorld(glm::vec3 translation)
{
    _view[3] += glm::vec4(translation, 0.0);
}

void Camera::setTranslationWorld(glm::vec3 translation)
{
    _view[3] = glm::vec4(translation, 1.0);
}

void Camera::transformWorld(glm::mat4 transform)
{
    _view = transform * _view;
}
