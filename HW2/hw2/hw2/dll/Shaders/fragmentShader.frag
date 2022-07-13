#version 430

in vec2 Texcoord;
in vec3 Normal;

uniform sampler2D TEX;

out vec4 texcolor;

void main()
{
	texcolor = texture2D(TEX, Texcoord);
}