#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aColor;

uniform float shift;

out vec4 vertex_color;

void main() {
    // make it revert, and shift
    gl_Position = vec4(aPos.x + shift - 0.5f,0.1f - aPos.y, aPos.z, 1.0f);
    vertex_color = vec4(aColor, 1.0f);
}