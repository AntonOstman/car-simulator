#pragma once

class Model{
    public:
        void bind(GLuint program);
        void generateBuffers(float* vertices, unsigned int num_vert);
        void drawTriangles();
        void drawLines();
    private:
        unsigned int _VBO;
        unsigned int _VAO;
        unsigned int _num_vert;
};
