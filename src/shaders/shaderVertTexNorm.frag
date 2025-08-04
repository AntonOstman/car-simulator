#version 330 core
out vec4 FragColor;
  
in vec3 fragNorm;
in vec2 fragTexCoord;

void main()
{
    FragColor = vec4(abs(normalize(fragNorm)), 1.0);
}
