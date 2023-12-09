#version 460

struct Material {
    sampler2D diffuse_texture;
    sampler2D specular_texture;
    sampler2D emission_texture;
    float shininess;
};

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

uniform samplerCube skybox_texture;
uniform float skybox_reflect_coeff;
uniform bool draw_border;
uniform vec3 view_pos;
uniform Material material;
uniform Light light;
uniform SpotLight spot_light;
uniform DirectionalLight directional_light;

vec3 calc_point_light(Light light, vec3 normal, vec3 frag_pos, vec3 viewDir)
{
    vec3 light_dir = normalize(light.light_pos - frag_pos);
    float dist = length(light.light_pos - frag_pos);
    // phong shading
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);
    // ambient
    vec3 ambient = vec3(texture(material.diffuse_texture, shared_tex_coords));
    // diffuse, max by 0 to avoid negative values
    vec3 diffuse = max(dot(normal, light_dir), 0.0f) * vec3(texture(material.diffuse_texture, shared_tex_coords));
    // specular, -light_dir because, light_dir is object->light, but for reflection we need light->object
    vec3 reflect_dir = reflect(-light_dir, normal);
    // 32 is the shininess
    // view_dir is the object->camera, reflect_dir is the object->light reflected, so we need to dot them
    vec3 specular = pow(max(dot(viewDir, reflect_dir), 0.0), material.shininess)
    * vec3(texture(material.specular_texture, shared_tex_coords));

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
    vec3 ambient = vec3(texture(material.diffuse_texture, shared_tex_coords));
    vec3 diffuse = max(dot(normal, light_dir), 0.0f) * vec3(texture(material.diffuse_texture, shared_tex_coords));
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess)
    * vec3(texture(material.specular_texture, shared_tex_coords));

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
    vec3 ambient = vec3(texture(material.diffuse_texture, shared_tex_coords));
    // diffuse
    vec3 diffuse = max(dot(normal, light_dir), 0.0f) * vec3(texture(material.diffuse_texture, shared_tex_coords));
    // specular
    vec3 reflect_dir = reflect(-light_dir, normal);
    vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess)
    * vec3(texture(material.specular_texture, shared_tex_coords));
    ambient *= spot_light.ambient;
    diffuse *= spot_light.diffuse;
    specular *= spot_light.specular;
    float dist = length(spot_light.light_pos - frag_pos);
    float attenuation = 1.0f / (spot_light.constant + spot_light.linear * dist + spot_light.quadratic * dist * dist);
    return attenuation * intensity * (ambient + diffuse + specular) * spot_light.light_color;
}

vec3 calc_skybox_reflect() {
    vec3 I = normalize(shared_frag_pos - view_pos);
    vec3 R = reflect(I, normalize(shared_normal));
    return texture(skybox_texture, R).xyz;
}

void main() {
    vec4 tex_color = texture(material.diffuse_texture, shared_tex_coords);
    if (tex_color.a < 0.1f) {
        discard;
    }
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    vec3 normal = normalize(shared_normal);
    vec3 emission = vec3(texture(material.emission_texture, shared_tex_coords));  // abandoned
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
    if (skybox_reflect_coeff != 0.0f && tex_color.a < 1.0f) {
        result = skybox_reflect_coeff * calc_skybox_reflect();
    }

    if (draw_border) {
        // completely black, so that it can draw black borders
        frag_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        frag_color = vec4(result, tex_color.a);
    }
}

