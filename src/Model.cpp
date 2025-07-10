
#include "glad/glad.h"
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "Model.hpp"

static GLenum lastError = 0;
static char lastErrorFunction[1024] = "";

void printError(const char *functionName)
{
   GLenum error;
   while (( error = glGetError() ) != GL_NO_ERROR)
   {
       if ((lastError != error) || (strcmp(functionName, lastErrorFunction)))
       {
	       fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
	       strcpy(lastErrorFunction, functionName);
	       lastError = error;
       }
   }
}

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
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
    printError("Model::bind");
}

void Model::draw()
{
    glDrawArrays(GL_TRIANGLES, 0, _num_vert);
}
