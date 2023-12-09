#version 460 core

in vec4 gs_out_frag_color;

out vec4 frag_color;

void main() {
    // hardcode to green
    frag_color = gs_out_frag_color;
}