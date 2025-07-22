#version 330 core
  
in vec3 in_Position;
uniform vec4 outColor; // we set this variable in the OpenGL code.
uniform mat4 mvp; // we set this variable in the OpenGL code.

out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    // mat4 rot = mat4(0,  1, 0, 0,
    //                -1, 0, 0, 0,
    //                0,  0, 1, 0,
    //                0,  0, 0, 1);
    gl_Position = mvp * vec4(in_Position, 1.0); // see how we directly give a vec3 to vec4's constructor
    // gl_Position = rot * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    // gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = outColor; // set the output variable to a dark-red color
}
