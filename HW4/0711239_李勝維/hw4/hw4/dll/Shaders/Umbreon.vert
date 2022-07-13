#version 430

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 M, V, P;

out vec2 uvs;
out vec3 worldNormals;
out vec3 worldPoses;

void main()
{
	gl_Position = V * M * vec4(in_position, 1.0); // in view space not in clip space!
	worldPoses = mat3(P * V * M) * in_position;
	worldNormals = mat3(transpose(inverse(M))) * in_normal;
	uvs = texcoord;
}
