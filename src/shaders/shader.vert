#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
  
out vec4 vertexColor; // specify a color output to the fragment shader
uniform vec4 ourColor; // we set this variable in the OpenGL code.
uniform mat4 rotate; // we set this variable in the OpenGL code.

void main()
{
    gl_Position = rotate * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    // gl_Position = rotate * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = ourColor; // set the output variable to a dark-red color
}
