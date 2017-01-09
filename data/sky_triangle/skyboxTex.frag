#version 410 core

uniform samplerCube background;

in vec3 v_uv;

layout(location = 0) out vec4 color;

void main()
{
    color = texture(background, normalize(v_uv));
}
