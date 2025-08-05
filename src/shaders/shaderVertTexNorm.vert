#version 330 core
  
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fragTexCoord;
out vec3 fragNormWorld;
out vec3 vertexPosWorld;
out vec3 lightPos;

void main()
{

    fragTexCoord = texCoord; // set the output variable to a dark-red color
    fragNormWorld = mat3(model) * norm; // set the output variable to a dark-red color
    vertexPosWorld = vec3(model * vec4(inPosition, 1.0));
    gl_Position = projection * view * model * vec4(inPosition, 1.0); // see how we directly give a vec3 to vec4's constructor
}
