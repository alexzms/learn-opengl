//
// Created by alexzms on 2023/10/27.
//

#ifndef LEARN_COLOR_LIGHT_MATERIAL_H
#define LEARN_COLOR_LIGHT_MATERIAL_H
#include "glm/glm.hpp"
struct Light {
    glm::vec3 light_color;
    glm::vec3 light_pos;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    glm::vec3 light_color;
    glm::vec3 light_pos;
    glm::vec3 light_dir;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float cut_off;
    float outer_cut_off;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight{
    glm::vec3 light_color;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct Material {
//    glm::vec3 ambient;
    // this is a texture id
    unsigned int diffuse_texture;
    unsigned int specular_texture;
    unsigned int emission_texture;
    float shininess;
};
#endif //LEARN_COLOR_LIGHT_MATERIAL_H
