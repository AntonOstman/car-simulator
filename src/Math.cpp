#include "Math.hpp"

/*
   Returns a symmetric perspective matrix.
*/

glm::mat4 perspective_matrix(float near, float far, float right, float top){
    float n = near;
    float f = far;
    float t = top;
    float r = right;
     
    // Symmetric frustrum
    float l = -r; // left
    float b = -t; // bottom

    glm::mat4 perspective = glm::mat4(
            n/r, 0, 0,           0,
            0, n/t, 0,           0,
            0, 0, -(f+n)/(f-n), -2*f*n/(f-n),
            0, 0, -1,            0
            );

    return perspective;
}
