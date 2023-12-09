#version 460

in vec2 shared_tex_coord;
out vec4 out_color;

uniform sampler2D tex;
uniform uint screen_width;
uniform uint screen_height;

const float offset = 1.0 / 300.0;

//float kernel[9] = float[](
//    1.0 / 16, 2.0 / 16, 1.0 / 16,
//    2.0 / 16, 4.0 / 16, 2.0 / 16,
//    1.0 / 16, 2.0 / 16, 1.0 / 16
//);

float kernel[9] = float[](
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // 左上
    vec2( 0.0f,    offset), // 正上
    vec2( offset,  offset), // 右上
    vec2(-offset,  0.0f),   // 左
    vec2( 0.0f,    0.0f),   // 中
    vec2( offset,  0.0f),   // 右
    vec2(-offset, -offset), // 左下
    vec2( 0.0f,   -offset), // 正下
    vec2( offset, -offset)  // 右下
);


void main() {
    vec3 color_vec3 = vec3(texture(tex, shared_tex_coord).xyz);
    float gamma = 2.2f;
    float gamma_inv = 0.4545;
    color_vec3 = pow(color_vec3, vec3(gamma_inv));
    // original
    out_color = vec4(color_vec3, 1.0f);
    // reverse color
//    out_color = vec4(1.0f - color_vec3), 1.0f);
    // gray
//    float average = 0.2126 * color_vec3.r + 0.7152 * color_vec3.g + 0.0722 * color_vec3.b;
//    out_color = vec4(average, average, average, 1.0f);
    // kernel
//    vec3 texture_sample[9];
//    for (int i = 0; i < 9; i++) {
//        texture_sample[i] = vec3(texture(tex, shared_tex_coord.st + offsets[i]).xyz);
//    }
//    vec3 col = vec3(0.0f);
//    for (int i = 0; i < 9; i++) {
//        col += texture_sample[i] * kernel[i];
//    }
//    out_color = vec4(col.xyz, 1.0f);
    // edge crop
//    if (gl_FragCoord.x < 100 || gl_FragCoord.x > screen_width - 100
//        || gl_FragCoord.y < 100 || gl_FragCoord.y > screen_height - 100) {
//        out_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
//    }
}