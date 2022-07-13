#version 430

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

uniform mat4 M, V, P;

out vec3 worldNormal;
out vec3 worldPos;

void main() 
{
	gl_Position = P * V * M * vec4(in_position, 1.0);
	worldPos = mat3(P * V * M) * in_position;
	// for normal space convertion, see: https://computergraphics.stackexchange.com/questions/1502/why-is-the-transposed-inverse-of-the-model-view-matrix-used-to-transform-the-nor
	worldNormal = mat3(transpose(inverse(M))) * in_normal;
}