#version 330 core
  
layout(location = 0 ) in vec3 inPosition;
layout(location = 1 ) in vec2 texCoord;
layout(location = 2 ) in vec3 norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fragTexCoord;
out vec3 fragNorm;

void main()
{
    vec3 lightPos = vec3(0, 5, 0);

    fragTexCoord = texCoord; // set the output variable to a dark-red color
    fragNorm = mat3(model) * norm; // set the output variable to a dark-red color
    gl_Position = projection * view * model * vec4(inPosition, 1.0); // see how we directly give a vec3 to vec4's constructor
}
