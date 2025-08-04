#pragma once
#include <vector>
#include "Loader.hpp"

class Model{
    public:
        void bind(GLuint program);
        void generateBuffersVertColor(float* vertices, unsigned int num_vert);
        void generateBuffersVertNormalTex(const std::vector<Vertex> &vertices);
        void drawTriangles();
        void drawLines();
    private:
        unsigned int _VBO;
        unsigned int _VAO;
        unsigned int _num_vert;
};
