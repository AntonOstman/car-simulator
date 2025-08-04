#pragma once

#include <string>
#include <vector>
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float2.hpp"

// Array of structs
struct Vertex {
    glm::vec3 Pos;
    glm::vec2 TexCoord;
    glm::vec3 Normal;
};

// Struct of arrays
struct Vertices {
    std::vector<glm::vec3> Pos;
    std::vector<glm::vec3> Normal;
    std::vector<glm::vec2> TexCoord;
};

std::vector<Vertex> parseObj(std::string);
