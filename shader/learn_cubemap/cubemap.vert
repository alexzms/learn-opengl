#version 460

layout (location = 0) in vec3 a_pos;

out vec3 texture_direction;

uniform mat4 proj_mat;
uniform mat4 view_mat;

void main() {
    gl_Position = proj_mat * view_mat * vec4(a_pos.xyz, 1.0f);
    texture_direction = a_pos;
}