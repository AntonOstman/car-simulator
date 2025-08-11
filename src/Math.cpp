#include "Math.hpp"

/*
   Returns a symmetric perspective matrix.

   Symmetry gives
   left = -right
   bottom = -top
*/

glm::mat4 perspective_matrix(float near, float far, float right, float top){
    float n = near;
    float f = far;
    float t = top;
    float r = right;
     
    // Symmetric frustrum means tihs is true
    glm::mat4 perspective = glm::mat4(
            n/r, 0, 0,           0,
            0, n/t, 0,           0,
            0, 0, -(f+n)/(f-n), -2*f*n/(f-n),
            0, 0, -1,            0
            );

    return perspective;
}

glm::mat4 Rz(float angle){
    angle = glm::radians(angle);
    glm::mat4 rot = glm::mat4(cos(angle), -sin(angle), 0.0, 0.0,
                              sin(angle), cos(angle),  0.0, 0.0,
                              0.0,           0.0,      1.0, 0.0,
                              0.0,           0.0,      0.0, 1.0);
    return rot;
}

glm::mat4 Rx(float angle){
    angle = glm::radians(angle);
    glm::mat4 rot = glm::mat4(  1.0,    0.0,      0.0,        0.0,
                                0.0, cos(angle), -sin(angle), 0.0,
                                0.0, sin(angle), cos(angle),  0.0,
                                0.0,      0.0,      0.0,      1.0);
    return rot;
}

glm::mat4 Ry(float angle){
    angle = glm::radians(angle);
    glm::mat4 rot = glm::mat4(cos(angle),  0.0, sin(angle),  0.0,
                              0.0,         1.0,   0.0,       0.0,
                              -sin(angle), 0.0,  cos(angle), 0.0,
                              0.0,         0.0,   0.0,       1.0);
    return rot;
}

glm::mat4 scaled_eye(float scale){
    glm::mat4 eye = glm::mat4(scale, 0.0,    0.0,   0.0,
                              0.0,   scale,  0.0,   0.0,
                              0.0,   0.0,    scale, 0.0,
                              0.0,   0.0,    0.0,   1.0);
    return eye;
}
