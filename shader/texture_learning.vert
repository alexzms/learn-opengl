#version 460
layout (location = 0) in vec3 pos;
//layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;

//out vec4 our_color;
out vec2 our_tex_coord;

//uniform mat4 trans;
uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main() {
    gl_Position = proj_mat * view_mat * model_mat * vec4(pos, 1.0f);
//    our_color = vec4(color, 1.0f);
    our_tex_coord = tex_coord;
}