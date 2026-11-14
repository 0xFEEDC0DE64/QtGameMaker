#version 440

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 v_texcoord;

layout(push_constant) uniform Push {
    mat4 matrix;
} push;

void main()
{
    v_texcoord = texcoord;
    gl_Position = push.matrix * vec4(position, 1.0);
}
