#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "Loader.hpp"
#include "glad/glad.h"

struct Shader {
    GLuint program;
};

struct Mesh {
    GLuint VBO;
    GLuint VAO;
    unsigned int num_vert;
};

class RenderingSystem{
    public:

        void static init();
        Shader static createShader(std::string frag, std::string vert);
        Mesh static createMesh(const std::vector<Vertex> &vertices);
        void static setUniforms(const GLuint& program, const glm::mat4& modelp, const glm::mat4& view, const glm::mat4& projection);
        void static drawLines(Mesh& mesh, GLuint program);
        void static drawTriangles(Mesh& mesh, GLuint program);
    private:
};
