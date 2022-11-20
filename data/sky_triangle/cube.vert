#version 150 core

in vec3 a_vertex;


void main()
{
	vec2 uv = vec2(0, floor(float(gl_InstanceID))) * 2.0;

	vec3 v = a_vertex;
	v.xz  += uv;

	gl_Position = vec4(v, 1.0); 
}
