#version 410 core

uniform mat4 inverseViewProjection;
uniform mat4 model;
uniform vec3 eye;

in vec2 in_vertex;

out vec2 v_uv;
out vec4 v_ray;

void main()
{
    gl_Position = vec4(in_vertex, 0.0, 1.0);

    v_uv = in_vertex.xy;
    
    v_ray =  inverseViewProjection * model * vec4(vec3(in_vertex, 0.0), 1.0);
}
