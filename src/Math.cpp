#include "Math.hpp"
#include <math.h>
#include <iostream>
#include <unordered_map>

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

bool isclose(float ref_val, float true_val, float rel_tol, float abs_tol)
{
    float diff = abs(ref_val - true_val);
    float max_val = std::fmax(abs(ref_val), true_val);

    if ((diff < abs_tol) || (diff < rel_tol * max_val))
    {
        return true;
    }

    return false;
}


uint32_t hash(std::int32_t x, std::int32_t y, std::int32_t seed){

    std::uint32_t h = x * 374761393u + y * 668265263u + seed * 700001u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h = h ^ (h >> 16);
    return h;
}

double random(std::int64_t x, std::int64_t y, std::int64_t seed){
    return ((double)(hash(x,y,seed) & 0xFFFFFFFF)) / ((double) 0xFFFFFFFF);
}


float S(float t){
    return 3.0*t*t - 2.0*t*t*t;
}

// float interpolate(int x,int y, float ul, float ur, float ll, float lr){
//
//     int gxidx = floor(x);
//     int gyidx = floor(y);
//
//     glm::vec3 llvec = glm::vec3(random(x,y,seed));
//
//     ll = np.dot(grid[gxidx,gyidx,:], np.array([gx - gxidx, gy - gyidx])) 
//     lr = np.dot(grid[gxidx+1,gyidx,:], np.array([gx - (gxidx+1), gy - gyidx])) 
//     ul = np.dot(grid[gxidx,gyidx+1,:], np.array([gx - gxidx, gy - (gyidx+1)])) 
//     ur = np.dot(grid[gxidx+1,gyidx+1,:], np.array([gx - (gxidx+1), gy - (gyidx+1)])) 
//
//     // ul = np.clip(ul, -1,1)
//     // ur = np.clip(ur, -1,1)
//     // ll = np.clip(ll, -1,1)
//     // lr = np.clip(lr, -1,1)
//
//     float L = ll + S(x - floor(x))*(lr - ll);
//     float U = ul + S(x - floor(x))*(ur - ul);
//
//     float noise = L + S(y - floor(y))*(U - L);
//
//     return noise ;
// }

/*
   returns Height value for a given x y coordinate
*/
double perlin_noise(int x, int y)
{
    return x*y;
}
