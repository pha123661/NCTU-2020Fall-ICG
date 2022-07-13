#version 430

in vec2 uv;
in vec3 worldNormal;
in vec3 worldPos;

uniform vec3 worldLight;
uniform sampler2D texture;

out vec4 color;

void main()
{
    vec3 to_light = normalize(worldLight - worldPos);
    float level = dot(to_light, worldNormal);
    float intensity;

    if (level > 0.95)
        intensity = 1;
    else if (level > 0.75)
        intensity = 0.8;
    else if (level > 0.50) 
        intensity = 0.6;
    else if (level > 0.25) 
        intensity = 0.4;
    else 
        intensity = 0.2;
    color = texture2D(texture, uv) * intensity;
}