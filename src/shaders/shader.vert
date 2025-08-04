#version 330 core
  
layout(location = 0 ) in vec3 inPosition;
layout(location = 1 ) in vec3 inColor;

uniform mat4 mvp;

out vec3 vertexColor;

void main()
{

    gl_Position = mvp * vec4(inPosition, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = inColor; // set the output variable to a dark-red color
}
