#version 330 core



in vec4 colour;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;
in vec2 TexCoords;

out vec4 FragColor;
vec4 outColour;

uniform vec3 cameraPos;
uniform sampler2D tex;
uniform bool use_texture;

const float ambient = .9;
 // maybe get rid of some of the lighting stuff to increase performance... 
void main()
{
    vec3 diffuseC = colour.xyz * max(dot(-lightDir, normal), 0.0);
    vec3 ambC = colour.xyz*ambient;
    vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)!
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,lightDir),0.0);
    float intensity = dot(normalize(lightDir), normal);
    vec3 specC = .8*vec3(1.0,1.0,1.0)*pow(spec,4);
    vec3 oColor = ambC+diffuseC+specC;

    if(intensity >= 0.95)
    {
        outColour.xyz = 1.12 * vec3(colour.xyz);
    }

    else if(intensity >= 0.5)
    {
        outColour.xyz = 0.94 * vec3(colour.xyz);
    }

    else if(intensity >= 0.25)
    {
        outColour.xyz = 0.73 * vec3(colour.xyz);
    }

    else if(intensity <= 0.05)
    {
        outColour.xyz = .5 * vec3(colour.xyz);
    }

    else
    {
        outColour.xyz = 0.35 * vec3(colour.xyz);
    }

    outColour.a = colour.a;
    if(outColour.a < 0.08)
    {
        discard;
    }

    FragColor = outColour;

    if(use_texture)
    {
        FragColor = texture(tex, TexCoords);
    }
    //FragColor.a = 0.6;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2)); // gamma correction
}
