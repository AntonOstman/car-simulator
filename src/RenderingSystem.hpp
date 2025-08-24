#pragma once

#include "EntityComponentSystem.hpp"
#include "Loader.hpp"
#include "UI.hpp"

class RenderingSystem{
    public:
        Mesh static createMesh(const std::vector<Vertex> &vertices);

        ShaderComp static createShader(std::string frag, std::string vert);
        void static update(ECS& ecs, UIsettings settings);
        void static init();
    private:
        void static setUniforms(const GLuint& program, const glm::mat4& modelp, const glm::mat4& view, const glm::mat4& projection);
        void static drawLines(Mesh& mesh, GLuint program);
        void static drawTriangles(Mesh& mesh, GLuint program);
    public:
    private:
};
