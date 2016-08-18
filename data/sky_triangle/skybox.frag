// according GLSL version to OpenGL 3.2 core profile
#version 150 core

uniform samplerCube background;
uniform vec3 eye;

in vec3 v_uv;

out vec4 out_color;

void main()
{
    out_color = texture(background, normalize(v_uv));
}
