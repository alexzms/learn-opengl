#version 460
//in vec4 our_color;
in vec2 our_tex_coord;

out vec4 frag_color;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float alpha;
uniform float mix_value;
uniform bool draw_border;

void main() {
    vec4 tex0 = texture(texture0, our_tex_coord);
    vec4 tex1 = texture(texture1, vec2(our_tex_coord.s, our_tex_coord.t));
    vec4 tex_color = mix(tex0, tex1, mix_value);
//    vec4 tex_color = tex0;
    float tex_alpha = tex_color.a;
    if (draw_border) {
        frag_color = vec4(0.0f, 0.0f, 0.0f, alpha);
    } else {
        frag_color = vec4(tex_color.rgb, tex_alpha * alpha);
    }
}