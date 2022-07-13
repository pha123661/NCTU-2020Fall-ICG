#version 430

in vec3 worldNormal;
in vec3 worldPos;

uniform vec3 worldCam;
uniform vec4 blue;

out vec4 color;

void main()
{
    vec3 to_cam = normalize(worldCam - worldPos);
    float intensity = 1.0f - dot(to_cam, worldNormal);
    intensity = pow(intensity , 10); // strengthening edges
    color = intensity * blue;
}