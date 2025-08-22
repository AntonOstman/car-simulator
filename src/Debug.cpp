#include "glad/glad.h"
#include <GL/glext.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

static GLenum lastError = 0;
static char lastErrorFunction[1024] = "";

void printActiveUniforms(unsigned int program)
{
    int num = -1;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num);
    std::cout << "Active uniforms \n" << num << std::endl;
}


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

void printvec(glm::vec3 p)
{
    std::cout << "x: " <<p.x << " y: " << p.y << " z: " << p.z << std::endl;
}

void printvec(glm::vec2 p)
{
    std::cout << "x: " <<p.x << " y: " << p.y << std::endl;
}
