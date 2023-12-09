#version 460

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct Light {
    vec3 light_color;
    vec3 light_pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 light_color;
    vec3 light_pos;
    vec3 light_dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cut_off;
    float outer_cut_off;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 light_color;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 shared_tex_coords;
in vec3 shared_normal;
in vec3 shared_frag_pos;

out vec4 frag_color;

float shininess = 32.0f;
uniform vec3 view_pos;
uniform Light light;
uniform SpotLight spot_light;
uniform DirectionalLight directional_light;
uniform float overall_light_enhance = 1.0f;

vec3 calc_point_light(Light light, vec3 normal, vec3 frag_pos, vec3 viewDir)
{
    vec3 light_dir = normalize(light.light_pos - frag_pos);
    float dist = length(light.light_pos - frag_pos);
    // phong shading
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    // ambient
    vec3 ambient = vec3(texture(texture_diffuse1, shared_tex_coords).xyz);
    // diffuse, max by 0 to avoid negative values
    vec3 diffuse = max(dot(normal, light_dir), 0.0f) * vec3(texture(texture_diffuse1, shared_tex_coords).xyz);
    // specular, -light_dir because, light_dir is object->light, but for reflection we need light->object
    vec3 reflect_dir = reflect(-light_dir, normal);
    // 32 is the shininess
    // view_dir is the object->camera, reflect_dir is the object->light reflected, so we need to dot them
    vec3 specular = pow(max(dot(viewDir, reflect_dir), 0.0), shininess)
    * vec3(texture(texture_specular1, shared_tex_coords).xyz);

    ambient *= light.ambient;
    diffuse *= light.diffuse;
    specular *= light.specular;
    return attenuation * (ambient + diffuse + specular) * light.light_color;
}

vec3 calc_directional_light(DirectionalLight directional_light, vec3 normal, vec3 view_dir) {
    // here we do a -direction because the light_dir should be frag->light_source, while direction is source->frag
    vec3 light_dir = normalize(-directional_light.direction);
    vec3 reflect_dir = reflect(-light_dir, normal);
    // ambient
    vec3 ambient = vec3(texture(texture_diffuse1, shared_tex_coords).xyz);
    vec3 diffuse = max(dot(normal, light_dir), 0.0f) * vec3(texture(texture_diffuse1, shared_tex_coords).xyz);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), shininess)
    * vec3(texture(texture_specular1, shared_tex_coords).xyz);

    ambient *= directional_light.ambient;
    diffuse *= directional_light.diffuse;
    specular *= directional_light.specular;
    return (ambient + diffuse + specular) * directional_light.light_color;
}

vec3 calc_spot_light(SpotLight spot_light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
    vec3 light_dir = normalize(spot_light.light_pos - frag_pos);
    // negative for frag->light
    float theta = dot(light_dir, normalize(-spot_light.light_dir));
    // cut_off is the angle of the spotlight
    float epsilon = spot_light.cut_off - spot_light.outer_cut_off;
    float intensity = clamp((theta - spot_light.outer_cut_off) / epsilon, 0.0f, 1.0f);
    // ambient
    vec3 ambient = vec3(texture(texture_diffuse1, shared_tex_coords).xyz);
    // diffuse
    vec3 diffuse = max(dot(normal, light_dir), 0.0f) * vec3(texture(texture_diffuse1, shared_tex_coords).xyz);
    // specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), shininess)
    * vec3(texture(texture_specular1, shared_tex_coords).xyz);
    ambient *= spot_light.ambient;
    diffuse *= spot_light.diffuse;
    specular *= spot_light.specular;
    float dist = length(spot_light.light_pos - frag_pos);
    float attenuation = 1.0f / (spot_light.constant + spot_light.linear * dist + spot_light.quadratic * dist * dist);
    return attenuation * intensity * (ambient + diffuse + specular) * spot_light.light_color;
}

float linear_depth(float depth_original) {
    float near = 0.1f;
    float far = 28.0f;
    float z = depth_original * 2.0f - 1.0f;
    return (2.0f * near * far) / (far + near - z * (far - near));
}

void main() {
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    vec3 normal = normalize(shared_normal);
    vec3 view_dir = normalize(view_pos - shared_frag_pos);
    {   // section of point light
        result += calc_point_light(light, normal, shared_frag_pos, view_dir);
    }
    {   // section of directional light
        result += calc_directional_light(directional_light, normal, view_dir);
    }
    {   // section of spotlight

        result += calc_spot_light(spot_light, normal, shared_frag_pos, view_dir);
    }

    result *= overall_light_enhance;

    frag_color = vec4(result, 1.0f);
//    frag_color = vec4(texture(texture_specular1, shared_tex_coords).xyz, 1.0f);
//    frag_color = vec4(vec3(pow(linear_depth(gl_FragCoord.z) / 5.0f, 3.0f)), 1.0f);
}

