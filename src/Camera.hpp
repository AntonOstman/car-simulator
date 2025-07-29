#pragma once

#include <glm/mat4x4.hpp>

class Camera{

public:
private:
    glm::mat4 _view;
    glm::vec2 _cur_angle;

public:
    Camera();

    void initCamera();
    void rotateRelative(glm::vec2 angle);

    void translate(glm::vec3 translation);

    void transform(glm::mat4 transform);
    glm::mat4 getView();
    void moveForward(float speed);
    void rotate(double anglex, double angley);
    void moveBack(float speed);
    void moveLeft(float speed);
    void moveRight(float speed);

    void moveUp(float speed);
    void moveDown(float speed);
private:

};
