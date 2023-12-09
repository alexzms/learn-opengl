#version 460

in vec3 texture_direction;
out vec4 frag_color;

uniform samplerCube cube_texture;

void main() {
    frag_color = texture(cube_texture, texture_direction);
}