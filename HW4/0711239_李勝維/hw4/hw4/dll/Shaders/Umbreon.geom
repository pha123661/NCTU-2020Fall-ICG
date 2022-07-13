#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 P;
uniform float time;

in vec2 uvs[];
in vec3 worldNormals[];
in vec3 worldPoses[];

out vec2 uv;
out vec3 worldNormal;
out vec3 worldPos;

vec4 displacement(vec4 pos, vec3 normal)
{
	vec3 tmp = normal * (time / 2);
	return pos + vec4(tmp, 1);
}

void main()
{
	vec3 cb = vec3(gl_in[1].gl_Position) - vec3(gl_in[2].gl_Position);
	vec3 ca = vec3(gl_in[0].gl_Position) - vec3(gl_in[2].gl_Position);
	vec3 normal = normalize(cross(cb, ca));

	for(int i=0;i<3;i++)
	{
		gl_Position = P * displacement(gl_in[i].gl_Position, normal);
		uv = uvs[i];
		worldNormal = worldNormals[i];
		worldPos = worldPoses[i];
		EmitVertex();
	}
	EndPrimitive();
}