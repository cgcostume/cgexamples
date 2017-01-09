#version 150 core

uniform samplerCube cubemap;
uniform sampler2D cubeExampleTexture;
uniform vec3 eye;

in vec2 v_uv;
in vec4 v_ray;

out vec4 out_color;

void main()
{
    vec3 stu = normalize(v_ray.xyz - eye);
    
    vec4 skyTriangleColor = texture(cubemap, stu);
    vec4 diff = abs(skyTriangleColor - texture(cubeExampleTexture, v_uv));
    
    //out_color = smoothstep(vec4(skyTriangleColor.rgb,1.0), diff, vec4(diff == vec4(0.0)));
    
    out_color = diff + vec4(skyTriangleColor.rgb, 0.3);
}

