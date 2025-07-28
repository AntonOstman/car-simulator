#version 330 core
  
in vec3 in_Position;
uniform vec4 outColor;
uniform mat4 mvp;

out vec4 vertexColor;

void main()
{

    gl_Position = mvp * vec4(in_Position, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = outColor; // set the output variable to a dark-red color
}
