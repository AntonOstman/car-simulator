#include "glad/glad.h"
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "Model.hpp"
#include "Debug.hpp"
#include "Loader.hpp"

void Model::generateBuffersVertNormalTex(const std::vector<Vertex> &vertices)
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);  
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Allocate memory
    // num float * num verts * num colors
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    _VAO = VAO;
    _VBO = VBO;
    _num_vert = vertices.size();
    printError("Model::generateBuffers");

    // Vertex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Pos)));
    printError("Model::bind::vertex");

    // Tex
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoord)));
    printError("Model::bind::tex");

    // Tex
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
    printError("Model::bind::normal");

    glBindVertexArray(0);
}

void Model::generateBuffersVertColor(float* vertices, unsigned int num_vert)
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);  
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Allocate memory
    // num float * num verts * num colors
    glBufferData(GL_ARRAY_BUFFER, 3 * num_vert * 2 * sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);
    _VAO = VAO;
    _VBO = VBO;
    _num_vert = num_vert;
    printError("Model::generateBuffers");

    // Vertex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    printError("Model::bind::vertex");

    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    printError("Model::bind::color");

    glBindVertexArray(0);
}

void Model::bind(GLuint program)
{
    printError("before Model::bind");
    glBindVertexArray(_VAO);
    glUseProgram(program); // Just in case
}

/*
   Draw VBO from bound model
   GL_TRIANGLES for triagle model
   GL_LINES for line model
*/

void Model::drawTriangles()
{
    // glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, _num_vert);
}

void Model::drawLines()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINES, 0, _num_vert);
}
