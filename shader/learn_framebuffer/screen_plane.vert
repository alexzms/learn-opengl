#version 460

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coord;

out vec2 shared_tex_coord;

void main() {
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    shared_tex_coord = tex_coord;
}