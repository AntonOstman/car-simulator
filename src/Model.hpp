#pragma once
#include <vector>
#include "Loader.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glad/glad.h"

class Model{
    public:
        void bind(GLuint program);
        void generateBuffersVertColor(float* vertices, unsigned int num_vert);
        void generateBuffersVertNormalTex(const std::vector<Vertex> &vertices);
        void drawTriangles();
        void drawLines();
    public:
        glm::mat4 getModelToWorld();
        // glm::mat4 setModelToWorld();
    public:
        glm::mat4 _modelToWorld;
    private:
        unsigned int _VBO;
        unsigned int _VAO;
        unsigned int _num_vert;
};
