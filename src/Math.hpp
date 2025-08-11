#pragma once
#include <glm/glm.hpp>

glm::mat4 perspective_matrix(float near, float far, float right, float top);
glm::mat4 Rz(float angle);
glm::mat4 Rx(float angle);
glm::mat4 Ry(float angle);
glm::mat4 scaled_eye(float scale);
