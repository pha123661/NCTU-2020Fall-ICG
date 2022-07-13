#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 Texcoord;
out vec3 Normal;

void main()
{
  gl_Position = P * V * M * vec4(position, 1.0);
  Texcoord = in_texcoord;
  Normal = in_normal;
}
