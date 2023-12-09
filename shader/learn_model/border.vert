#version 460
layout (location = 0) in vec3 position;

uniform mat4 proj_mat;
uniform mat4 view_mat;
uniform mat4 model_mat;

void main() {
    gl_Position = proj_mat * view_mat * model_mat * vec4(position, 1.0f);
}