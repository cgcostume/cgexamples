#version 150 core

uniform samplerCube cubemap;
uniform mat4 inverseViewProjection;

in vec2 v_uv;
//in vec4 v_ray;

out vec4 out_color;

void main()
{
    vec4 ray = inverseViewProjection * vec4(v_uv, 1.0, 1.0);
    vec3 stu = normalize(ray.xyz / ray.w);
	//vec3 stu = normalize(v_ray.xyz);

	out_color = vec4(texture(cubemap, stu).rgb, 1.0);
}
