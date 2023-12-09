//
// Created by alexzms on 2023/10/10.
//

#ifndef LEARN_COLOR_POSDATA_H
#define LEARN_COLOR_POSDATA_H
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <cmath>
#include <vector>
float genshin_vertices[] = {
        // pos, texture_pos, normal
        -0.1875f, -0.33f, -0.4f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
        0.1875f, -0.33f, -0.4f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
        0.1875f,  0.33f, -0.4f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        0.1875f,  0.33f, -0.4f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        -0.1875f,  0.33f, -0.4f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
        -0.1875f, -0.33f, -0.4f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

        -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        0.1875f, -0.33f,  0.4f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        0.1875f,  0.33f,  0.4f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        0.1875f,  0.33f,  0.4f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.1875f,  0.33f,  0.4f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

        -0.1875f,  0.33f,  0.4f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.1875f,  0.33f, -0.4f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        -0.1875f,  0.33f,  0.4f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

        0.1875f,  0.33f,  0.4,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        0.1875f,  0.33f, -0.4,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        0.1875f, -0.33f, -0.4,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        0.1875f, -0.33f, -0.4,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
        0.1875f, -0.33f,  0.4,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
        0.1875f,  0.33f,  0.4,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

        -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
        0.1875f, -0.33f, -0.4f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
        0.1875f, -0.33f,  0.4f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        0.1875f, -0.33f,  0.4f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
        -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f,0.0f, -1.0f,  0.0f,

        -0.1875f,  0.33f, -0.4f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        0.1875f,  0.33f, -0.4f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
        0.1875f,  0.33f,  0.4f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        0.1875f,  0.33f,  0.4f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        -0.1875f,  0.33f,  0.4f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
        -0.1875f,  0.33f, -0.4f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
};

float wood_box_vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

float ground_vertices[] = {
        // Positions          // Texture Coords                // normal(up)
        1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,

        1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        1.0f,  0.0f, -1.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f,
};

glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
};

std::vector<glm::vec3> vegetation_pos = {
        glm::vec3(-1.5f,  0.0f, -0.48f),
        glm::vec3( 1.5f,  0.0f,  0.51f),
        glm::vec3( 0.0f,  0.0f,  0.7f),
        glm::vec3(-0.3f,  0.0f, -2.3f),
        glm::vec3( 0.5f,  0.0f, -0.6f),
};

float screen_square_vertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
};

#define GLASS_NUM 800

std::vector<glm::vec3> glass_pos = {};
std::vector<glm::vec3> glass_scale = {};
std::vector<glm::vec3> glass_rotate = {};
// randomly generate glass position, within the range of (-20, 20)
// randomly generate glass scale, within the range of (0.5, 1.5)
// randomly generate glass rotate, within the range of (0, 360)
//void generate_glass_info() {
//    glass_pos.clear();
//    glass_scale.clear();
//    glass_rotate.clear();
//    for (int i = 0; i < GLASS_NUM; i++) {
//        float x = (float)(rand() % 400 - 200) / 10.0f;
//        float y = (float)(rand() % 400 - 200) / 10.0f;
//        float z = (float)(rand() % 400 - 200) / 10.0f;
//        glass_pos.emplace_back(x, y, z);
//        float scale = (float)(rand() % 10 + 5) / 10.0f;
//        glass_scale.emplace_back(scale, scale, scale);
//        float rotate_x = (float)(rand() % 360);
//        float rotate_y = (float)(rand() % 360);
//        float rotate_z = (float)(rand() % 360);
//        glass_rotate.emplace_back(rotate_x, rotate_y, rotate_z);
//    }
//}
void generate_glass_info(glm::vec3 target_pos) {
    // generate glass that is well-arranged within x, y, z within (-20, 20)
    glass_pos.clear();
    glass_scale.clear();
    glass_rotate.clear();
    auto sqrt_3_glass_num = (float)pow(GLASS_NUM, 1.0f / 3.0f);
    float square_length = 40.0f / sqrt_3_glass_num;
    for (int i = 0; i < sqrt_3_glass_num; i++) {
        for (int j = 0; j < sqrt_3_glass_num; j++) {
            for (int k = 0; k < sqrt_3_glass_num; k++) {
                float x = (float)(i - sqrt_3_glass_num / 2) * square_length;
                float y = (float)(j - sqrt_3_glass_num / 2) * square_length;
                float z = (float)(k - sqrt_3_glass_num / 2) * square_length;
                glass_pos.emplace_back(x, y, z);
                float scale = 1.0f;
                glass_scale.emplace_back(scale, scale, scale);
                glm::vec3 diff = target_pos - glm::vec3(x, y, z);
                // arctan2(y, x) return the angle between x-axis and the vector (x, y)
                float rotation_x = glm::degrees(atan2(diff.z, diff.y));
                float rotation_y = glm::degrees(atan2(diff.x, diff.z));
                float rotation_z = glm::degrees(atan2(diff.y, diff.x));
//            glass_rotate.emplace_back(rotation_x, rotation_y, rotation_z);
                glass_rotate.emplace_back(rotation_x, -rotation_z, rotation_y);
            }
        }
    }
}


std::vector<std::string> skybox_texture_paths =
        {
        "./textures/cube_texture/skybox/right.jpg",
        "./textures/cube_texture/skybox/left.jpg",
        "./textures/cube_texture/skybox/top.jpg",
        "./textures/cube_texture/skybox/bottom.jpg",
        "./textures/cube_texture/skybox/front.jpg",
        "./textures/cube_texture/skybox/back.jpg"
        };

GLfloat skybox_vertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};
#endif //LEARN_COLOR_POSDATA_H
