#version 460
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_tex_coords;
layout (location = 2) in vec3 in_normal;

out vec2 shared_tex_coords;
out vec3 shared_normal;
out vec3 shared_frag_pos;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;


void main() {
    gl_Position = proj_mat * view_mat * model_mat * vec4(in_pos, 1.0f);
    shared_tex_coords = in_tex_coords;
    // mathematically, normals should be transformed by the inverse transpose of the model matrix
    // normal_model = transpose(inverse(model_mat)) * normal_world
    shared_normal = vec3(transpose(inverse(model_mat)) * vec4(in_normal, 0.0f));
    shared_frag_pos = vec3(model_mat * vec4(in_pos, 1.0f));
}