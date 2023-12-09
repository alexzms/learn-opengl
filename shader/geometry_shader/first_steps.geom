#version 460 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec4 gs_out_frag_color;

void build_house(vec4 position, vec3 color)
{
    gs_out_frag_color = vec4(color.xyz, 1.0f);
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
    EmitVertex();
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
    // the last vertex is white, to let opengl to interpolate
    gs_out_frag_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    EmitVertex();
    EndPrimitive();
}

void draw_line(vec4 position) {
    gl_Position = position - vec4(0.1f, 0.0f, 0.0f, 0.0f);
    EmitVertex();
    gl_Position = position + vec4(0.1f, 0.0f, 0.0f, 0.0f);
    EmitVertex();
    EndPrimitive();
}


void main() {
    build_house(gl_in[0].gl_Position, gs_in[0].color);
}