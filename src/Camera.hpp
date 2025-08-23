#pragma once

#include <glm/mat4x4.hpp>
#include "EntityComponentSystem.hpp"

class CameraSystem{

public:
private:
    glm::mat4 _view;
    glm::vec2 _cur_angle;

public:
    void static rotateRelative(CameraComp& comp, glm::vec2 angle);
    void static translateWorld(CameraComp& comp, glm::vec3 translation);
    void static transformWorld(CameraComp& comp, glm::mat4 transform);
    glm::mat4 static getViewToWorld(CameraComp& comp);
    glm::mat4 static getWorldToView(CameraComp& comp);
    void static moveForward(CameraComp& comp, float speed);
    void static rotate(CameraComp& comp, double anglex, double angley);
    void static moveBack(CameraComp& comp, float speed);
    void static moveLeft(CameraComp& comp, float speed);
    void static moveRight(CameraComp& comp, float speed);
    void static moveUp(CameraComp& comp, float speed);
    void static moveDown(CameraComp& comp, float speed);
    void static setTranslationWorld(CameraComp& comp, glm::vec3 translation);
    void static init(CameraComp& comp);
    void static setPerspective(CameraComp& comp, float fov, float width, float height, float near, float far);
    glm::vec3 static getForwardWorld(CameraComp& comp);
    glm::vec3 static getWorldPos(CameraComp& comp);

private:

};
