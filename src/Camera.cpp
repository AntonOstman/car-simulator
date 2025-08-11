#define GLM_FORCE_SWIZZLE
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "EntityComponentSystem.hpp"

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

/*
   Returns the view to world camera matrix
*/

glm::mat4 CameraSystem::getViewToWorld(CameraComp& comp)
{
    return comp.view;
}

void CameraSystem::init(CameraComp& comp)
{
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::vec3 eye = glm::vec3(0, 0, -10);
    glm::vec3 lookTowards = glm::vec3(0, 0, 0);
 
    comp.cur_angle = glm::vec2(0, 0);
    comp.view = lookAt(up, eye, lookTowards);
    // comp.view = glm::lookAt(eye, lookTowards, up);
}

void CameraSystem::moveForward(CameraComp& comp, float speed)
{
    comp.view[3] -= speed * comp.view[2];
}

void CameraSystem::moveBack(CameraComp& comp, float speed)
{
    comp.view[3] += speed * comp.view[2];
}

void CameraSystem::moveLeft(CameraComp& comp, float speed)
{
    comp.view[3] -= speed * comp.view[0];
}

void CameraSystem::moveRight(CameraComp& comp,float speed)
{
    comp.view[3] += speed * comp.view[0];
}

void CameraSystem::moveUp(CameraComp& comp, float speed)
{
    comp.view[3] += speed * comp.view[1];
}

void CameraSystem::moveDown(CameraComp& comp, float speed)
{
    comp.view[3] -= speed * comp.view[1];
}

void CameraSystem::rotateRelative(CameraComp& comp, glm::vec2 angle)
{
    comp.cur_angle += angle;

    if (abs((int)comp.cur_angle.y) > 50.0)
    {
        comp.cur_angle.y -= angle.y;
    }
    glm::quat pitch = glm::angleAxis((float)glm::radians(comp.cur_angle.y), glm::normalize(glm::vec3(comp.view[0])));
    glm::quat yaw = glm::angleAxis((float)glm::radians(comp.cur_angle.x), glm::normalize(glm::vec3(0,1,0)));
    glm::quat rot = pitch * yaw;

    glm::mat4 cameraRot = glm::mat4(rot);
    comp.view[0] = cameraRot[0];
    comp.view[1] = cameraRot[1];
    comp.view[2] = cameraRot[2];
}

void CameraSystem::translateWorld(CameraComp& comp, glm::vec3 translation)
{
    comp.view[3] += glm::vec4(translation, 0.0);
}

void CameraSystem::setTranslationWorld(CameraComp& comp, glm::vec3 translation)
{
    comp.view[3] = glm::vec4(translation, 1.0);
}

void CameraSystem::transformWorld(CameraComp& comp, glm::mat4 transform)
{
    comp.view = transform * comp.view;
}
