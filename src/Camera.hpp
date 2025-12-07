#pragma once

#include <glm/mat4x4.hpp>

class Camera{


public:
    Camera();
    void rotateRelative(glm::vec2 angle);
    void translateWorld(glm::vec3 translation);
    void transformWorld(glm::mat4 transform);
    glm::mat4 getViewToWorld();
    glm::mat4 getWorldToView();
    void moveForward(float speed);
    void rotate(double anglex, double angley);
    void moveBack(float speed);
    void moveLeft(float speed);
    void moveRight(float speed);
    void moveUp(float speed);
    void moveDown(float speed);
    void setTranslationWorld(glm::vec3 translation);
    void setView();
    void setPerspective(float fov, float width, float height, float near, float far);
    glm::mat4 getPerspective();
    glm::vec3 getForwardWorld();
    glm::vec3 getWorldPos();

private:
    glm::mat4 _view;
    glm::mat4 _perspective;
    glm::vec2 _cur_angle;

};
