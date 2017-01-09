#version 150 core

uniform mat4 modelViewProjection;

out float g_h;

out vec2 g_uv;
out vec3 g_normal;

layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

void main()
{
	vec4 u = gl_in[1].gl_Position - gl_in[0].gl_Position;
	vec4 v = gl_in[2].gl_Position - gl_in[0].gl_Position;

	vec3 n = cross(normalize((modelViewProjection * u).xyz), normalize((modelViewProjection * v).xyz));

	gl_Position = modelViewProjection * gl_in[0].gl_Position;
	g_uv = vec2(0.0, 0.0);
	g_normal = n;
	g_h = 1;
	EmitVertex();

	gl_Position = modelViewProjection * gl_in[1].gl_Position;
	g_uv = vec2(1.0, 0.0);
	EmitVertex();

	gl_Position = modelViewProjection * gl_in[2].gl_Position;
	g_uv = vec2(0.0, 1.0);
	EmitVertex();

	gl_Position = modelViewProjection * vec4((gl_in[0].gl_Position + u + v).xyz, 1.0);
	g_uv = vec2(1.0, 1.0);
	EmitVertex();
}
