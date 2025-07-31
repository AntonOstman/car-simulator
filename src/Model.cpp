
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "Model.hpp"
#include "Debug.hpp"


void Model::generateBuffers(float* vertices, unsigned int num_vert)
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);  
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Allocate memory
    glBufferData(GL_ARRAY_BUFFER, 3 * num_vert * sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);
    _VAO = VAO;
    _VBO = VBO;
    _num_vert = num_vert;
    printError("Model::generateBuffers");
}

void Model::bind(GLuint program)
{
    printError("before Model::bind");
    glBindVertexArray(_VAO);
    glUseProgram(program); // Just in case

    unsigned int loc = glGetAttribLocation(program, "in_Position");
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(loc);
    printError("Model::bind");
}

/*
   Draw VBO from bound model
   GL_TRIANGLES for triagle model
   GL_LINES for line model
*/

void Model::drawTriangles()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, _num_vert);
}

void Model::drawLines()
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINES, 0, _num_vert);
}
