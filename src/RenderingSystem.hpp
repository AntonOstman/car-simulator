#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "Loader.hpp"
#include "glad/glad.h"

struct Shader {
    GLuint program;
};

struct RenderTransforms
{
    glm::mat4 modelToWorld;
    glm::mat4 worldToView;
    glm::mat4 projection;
};

enum class MeshType
{
    NONE,
    CUBE,
};

enum class ShaderType
{
    NONE,
    STANDARD,
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
        void static renderMesh(MeshType meshType, ShaderType shaderType, glm::mat4 modelToWorld, glm::mat4 worldToView, glm::mat4 projection);
        Mesh static getMesh(MeshType mesh);
        Shader static getShader(ShaderType shader);
    private:
};
