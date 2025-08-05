#version 330 core
out vec4 FragColor;
  
in vec3 fragNormWorld;
in vec2 fragTexCoord;
in vec3 vertexPosWorld;

void main()
{
    vec3 diffuse = vec3(1.0, 1.0, 1.0);
    vec3 ambient = vec3(0.2, 0.2, 0.2);

    vec3 lightPosWorld = vec3(10, 10, 0);
    float distance = length(lightPosWorld - vertexPosWorld);
    vec3 lightNormal = normalize(lightPosWorld - vertexPosWorld);

    float intensity = 1.0 / sqrt(distance) * max(dot(normalize(fragNormWorld), lightNormal), 0);
    vec3 color =  min(intensity * diffuse + ambient, vec3(1.0,1.0,1.0));

    FragColor = vec4(color, 1.0);
}
