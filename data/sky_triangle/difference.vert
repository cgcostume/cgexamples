#version 410 core

uniform mat4 inverseViewProjection;

in vec2 in_vertex;

out vec2 v_uv;
out vec4 v_ray;

const vec2 scaleFactor = vec2(0.5,0.5);

void main()
{
    gl_Position = vec4(in_vertex, -1.0, 1.0);
    
    v_uv = in_vertex.xy * scaleFactor + scaleFactor; // scale vertex to [0-1]
    
    v_ray =  inverseViewProjection * vec4(in_vertex, -1.0, 1.0);
}
