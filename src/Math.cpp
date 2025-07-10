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
