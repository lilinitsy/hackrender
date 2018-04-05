#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

const vec3 inLightDir = normalize(vec3(-1,1,-1));

out vec4 colour;
out vec3 normal;
out vec3 pos;
out vec3 lightDir;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 inColour;

void main()
{
    colour = inColour;
    pos = (view * model * vec4(aPos, 1.0)).xyz;
    lightDir = vec4(inLightDir, 0.0).xyz;
    vec4 norm4 = transpose(inverse(view * model)) * vec4(aNormal, 0.0);
    normal = normalize(norm4.xyz);
    TexCoords = aTexCoords;
//    gl_PointSize = 5.0;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = 4.0;
}
