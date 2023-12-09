#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "iostream"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "assimp/Importer.hpp"
#include "includes/PosData.h"
#include "Camera.h"

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

unsigned int load_texture(const std::string& texture_path, unsigned int texture_unit);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double unknown_offset, double scroll_offset);
void mouse_callback(GLFWwindow* window, double width_pos, double height_pos);
void process_input(GLFWwindow* window);

// use default camera init parameters
static Camera camera;

float prev_time = 0.0f;
float delta_time = 0.0f;
float width_mouse = 0.0f;
float height_mouse = 0.0f;
bool first_mouse = true;
bool should_put_light = false;
glm::vec3 light_init_pos = glm::vec3(1.2f, 1.0f, 2.0f);


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    stbi_set_flip_vertically_on_load(true);
    GLFWmonitor* glfWmonitor = glfwGetPrimaryMonitor();
    if (glfWmonitor == nullptr) {
        std::cout << "Failed to get primary monitor.." << std::endl;
        glfwTerminate();
        return -1;
    }
    const GLFWvidmode *glfWvidmode = glfwGetVideoMode(glfWmonitor);
    unsigned int RENDER_WIDTH = glfWvidmode->width;
    unsigned int RENDER_HEIGHT = glfWvidmode->height;

    GLFWwindow* window = glfwCreateWindow(RENDER_WIDTH, RENDER_HEIGHT,
                                          "Colors", glfWmonitor, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create glf windows.." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD loader.." << std::endl;
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    glm::mat4 model_mat = glm::mat4(1.0f);
    glm::mat4 view_mat = camera.GetViewMatrix();
    glm::mat4 proj_mat = glm::perspective(glm::radians(camera.Zoom),
                                          (float)RENDER_WIDTH / (float) RENDER_HEIGHT,
                                          0.1f, 100.0f);
    const Shader box_shader("./shader/color_learning/boxes.vert",
                            "./shader/color_learning/boxes.frag");
    const Shader light_shader("./shader/color_learning/light.vert",
                              "./shader/color_learning/light.frag");
    const Shader ground_shader("./shader/color_learning/ground.vert",
                               "./shader/color_learning/ground.frag");


    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // blend policy
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // depth policy(enable depth buffer)
    glEnable(GL_DEPTH_TEST);

    unsigned int wood_box_texture_id = load_texture("./textures/wood_box_specular.png", 0);
    unsigned int wood_box_specular_texture_id = load_texture("./textures/awesomeface.jpg", 1);
    unsigned int emission_texture_id = load_texture("./textures/emission_matrix.jpg", 2);
    unsigned int ground_texture_id = load_texture("./textures/genshin.png", 3);

//    // offset of "texture0", on gpu's texture processing unit, which is 0 for texture0
    // no need of this, we will have the color shown by diffuse color
//    box_shader.set_int("texture0", 1);
    Material box_material{};
    //    box_material.ambient = glm::vec3(0.0f, 0.1f, 0.06f);
    box_material.diffuse_texture = 0;
    box_material.specular_texture = 1;
    box_material.emission_texture = 2;
    box_material.shininess = 64.0f;
    Material ground_material{};
    ground_material.diffuse_texture = 3;
    ground_material.specular_texture = 3;
    ground_material.emission_texture = 3;
    ground_material.shininess = 16.0f;

    Light light{};
    light.light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    light.light_pos = light_init_pos;
    float enhance = 0.3;
    light.ambient = enhance * glm::vec3(0.4f, 0.4f, 0.4f);
    light.diffuse = enhance * glm::vec3(1.8f, 1.8f, 1.8f);
    light.specular = enhance * 1.3f * glm::vec3(2.9f, 2.9f, 2.9f);
    light.constant = 1.0f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;

    SpotLight spot_light{};
    float enhance_spot_light = 0.3;
    spot_light.light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    spot_light.light_pos = camera.Position;
    spot_light.light_dir = camera.Front;
    spot_light.ambient = enhance_spot_light * glm::vec3(0.0f, 0.0f, 0.0f);
    spot_light.diffuse = enhance_spot_light * glm::vec3(1.8f, 1.8f, 1.8f);
    spot_light.specular = enhance_spot_light * 1.3f * glm::vec3(2.9f, 2.9f, 2.9f);
    spot_light.cut_off = glm::cos(glm::radians(12.5f));
    spot_light.outer_cut_off = glm::cos(glm::radians(17.5f));
    spot_light.constant = 1.0f;
    spot_light.linear = 0.09f;
    spot_light.quadratic = 0.032f;

    DirectionalLight directional_light{};
    float enhance_directional_light = 0.2;
    directional_light.direction = glm::vec3(-0.2f, -1.0f, -3.0f);
    directional_light.ambient = enhance_directional_light * glm::vec3(0.4f, 0.4f, 0.4f);
    directional_light.diffuse = enhance_directional_light * glm::vec3(1.8f, 1.8f, 1.8f);
    directional_light.specular = enhance_directional_light * 1.3f * glm::vec3(2.9f, 2.9f, 2.9f);

    unsigned int boxes_VAO, boxes_VBO;
    unsigned int light_VAO;
    {
        // boxes
        glGenVertexArrays(1, &boxes_VAO);
        glBindVertexArray(boxes_VAO);
        glGenBuffers(1, &boxes_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, boxes_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wood_box_vertices), wood_box_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0); // pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float))); // tex
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float))); // normal
        glEnableVertexAttribArray(2);
        // light source
        glGenVertexArrays(1, &light_VAO);
        glBindVertexArray(light_VAO);
        // we use the same boxes_VBO as the boxes, because they have the same vertices, we will only change the model matrix
        glBindBuffer(GL_ARRAY_BUFFER, boxes_VBO);
        // only need to set the position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*) 0);
        glEnableVertexAttribArray(0);
    }
    unsigned int ground_VAO, ground_VBO;
    {
        // ground
        glGenVertexArrays(1, &ground_VAO);
        glBindVertexArray(ground_VAO);
        glGenBuffers(1, &ground_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, ground_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices), ground_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*) 0); // pos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*) (5 * sizeof(float))); // normal
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof (float), (void*) (3 * sizeof(float))); // tex
        glEnableVertexAttribArray(1);
    }
    {
        box_shader.use();
        box_shader.set_int("material.diffuse_texture", box_material.diffuse_texture);
        box_shader.set_int("material.specular_texture", box_material.specular_texture);
        box_shader.set_int("material.emission_texture", box_material.emission_texture);
        box_shader.set_float("material.shininess", box_material.shininess);
        box_shader.set_vec3("light.ambient", light.ambient);
        box_shader.set_vec3("light.diffuse", light.diffuse);
        box_shader.set_vec3("light.specular", light.specular);
        box_shader.set_float("light.constant", light.constant);
        box_shader.set_float("light.linear", light.linear);
        box_shader.set_float("light.quadratic", light.quadratic);
        box_shader.set_vec3("spot_light.ambient", spot_light.ambient);
        box_shader.set_vec3("spot_light.diffuse", spot_light.diffuse);
        box_shader.set_vec3("spot_light.specular", spot_light.specular);
        box_shader.set_float("spot_light.cut_off", spot_light.cut_off);
        box_shader.set_float("spot_light.outer_cut_off", spot_light.outer_cut_off);
        box_shader.set_float("spot_light.constant", spot_light.constant);
        box_shader.set_float("spot_light.linear", spot_light.linear);
        box_shader.set_float("spot_light.quadratic", spot_light.quadratic);
        box_shader.set_vec3("directional_light.light_color", directional_light.light_color);
        box_shader.set_vec3("directional_light.direction", directional_light.direction);
        box_shader.set_vec3("directional_light.ambient", directional_light.ambient);
        box_shader.set_vec3("directional_light.diffuse", directional_light.diffuse);
        box_shader.set_vec3("directional_light.specular", directional_light.specular);
        ground_shader.use();
        ground_shader.set_int("material.diffuse_texture", ground_material.diffuse_texture);
        ground_shader.set_int("material.specular_texture", ground_material.specular_texture);
        ground_shader.set_int("material.emission_texture", ground_material.emission_texture);
        ground_shader.set_float("material.shininess", ground_material.shininess);
        ground_shader.set_vec3("light.ambient", light.ambient);
        ground_shader.set_vec3("light.diffuse", light.diffuse);
        ground_shader.set_vec3("light.specular", light.specular);
        ground_shader.set_float("light.constant", light.constant);
        ground_shader.set_float("light.linear", light.linear);
        ground_shader.set_float("light.quadratic", light.quadratic);
        ground_shader.set_vec3("spot_light.ambient", spot_light.ambient);
        ground_shader.set_vec3("spot_light.diffuse", spot_light.diffuse);
        ground_shader.set_vec3("spot_light.specular", spot_light.specular);
        ground_shader.set_float("spot_light.cut_off", spot_light.cut_off);
        ground_shader.set_float("spot_light.outer_cut_off", spot_light.outer_cut_off);
        ground_shader.set_float("spot_light.constant", spot_light.constant);
        ground_shader.set_float("spot_light.linear", spot_light.linear);
        ground_shader.set_float("spot_light.quadratic", spot_light.quadratic);
        ground_shader.set_vec3("directional_light.direction", directional_light.direction);
        ground_shader.set_vec3("directional_light.ambient", directional_light.ambient);
        ground_shader.set_vec3("directional_light.diffuse", directional_light.diffuse);
        ground_shader.set_vec3("directional_light.specular", directional_light.specular);
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        auto current_time = (float)glfwGetTime();
        delta_time = current_time - prev_time;
        prev_time = current_time;

        process_input(window);
        directional_light.light_color = glm::vec3(sin((float)glfwGetTime()),
                                                  cos((float)glfwGetTime() / 2),
                                                  1-(cos((float)glfwGetTime())+sin((float)glfwGetTime() / 2))/2);
        glClearColor(directional_light.light_color.x, directional_light.light_color.y,
                     directional_light.light_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        proj_mat = glm::perspective(glm::radians(camera.Zoom),
                                    (float)RENDER_WIDTH / (float) RENDER_HEIGHT,
                                    0.1f, 100.0f);


        light.light_color = glm::vec3(1.0f, 1.0f, 1.0f);
        spot_light.light_pos = camera.Position;
        spot_light.light_dir = camera.Front;
        if (should_put_light) {
            light.light_pos = camera.Position + 2.0f * camera.Front;
            should_put_light = false;
        }

        // draw light source
        light_shader.use();
        view_mat = camera.GetViewMatrix();
        {
            light_shader.set_mat4("view_mat", view_mat);
            light_shader.set_mat4("proj_mat", proj_mat);
            light_shader.set_vec3("light_color", light.light_color);
            light_shader.set_mat4("model_mat",
                                  glm::scale(glm::translate(glm::mat4(1.0f), light.light_pos),
                                             glm::vec3(0.2f, 0.2f, 0.2f)));
        }
        glBindVertexArray(light_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // draw boxes
        box_shader.use();
        view_mat = camera.GetViewMatrix();
        {
            box_shader.set_mat4("view_mat", view_mat);
            box_shader.set_mat4("proj_mat", proj_mat);
            box_shader.set_vec3("light.light_pos", light.light_pos);
            box_shader.set_vec3("light.light_color", light.light_color);
            box_shader.set_vec3("spot_light.light_color", spot_light.light_color);
            box_shader.set_vec3("directional_light.light_color", directional_light.light_color);
            box_shader.set_vec3("spot_light.light_pos", spot_light.light_pos);
            box_shader.set_vec3("spot_light.light_dir", spot_light.light_dir);
            box_shader.set_vec3("view_pos", camera.Position);
        }
        glActiveTexture(GL_TEXTURE0 + box_material.diffuse_texture);
        glBindTexture(GL_TEXTURE_2D, wood_box_texture_id);
        glActiveTexture(GL_TEXTURE0 + box_material.specular_texture);
        glBindTexture(GL_TEXTURE_2D, wood_box_specular_texture_id);
        glActiveTexture(GL_TEXTURE0 + box_material.emission_texture);
        glBindTexture(GL_TEXTURE_2D, emission_texture_id);
        glActiveTexture(GL_TEXTURE0 + ground_material.diffuse_texture);
        glBindTexture(GL_TEXTURE_2D, ground_texture_id);
        glActiveTexture(GL_TEXTURE0 + ground_material.specular_texture);
        glBindTexture(GL_TEXTURE_2D, ground_texture_id);
        glActiveTexture(GL_TEXTURE0 + ground_material.emission_texture);
        glBindTexture(GL_TEXTURE_2D, ground_texture_id);
        glBindVertexArray(boxes_VAO);
        for (int i = 0; i != 10; ++i) {
            model_mat = glm::mat4(1.0f);
            model_mat = glm::translate(model_mat, cubePositions[i]);
            model_mat = glm::scale(model_mat, glm::vec3(1.2f, 1.2f, 1.2f));
//            model_mat = glm::rotate(model_mat,
//                                    30.0f * (float)glfwGetTime() * glm::radians((float)(i + 1)),
//                                    glm::vec3(sin(i), cos(i), tan(i)));
            glUniformMatrix4fv(glGetUniformLocation(box_shader.ID, "model_mat"), 1, GL_FALSE, glm::value_ptr(model_mat));
            int bias = i % 2;
//            glActiveTexture(GL_TEXTURE0 + bias);
//            glBindTexture(GL_TEXTURE_2D, wood_box_texture_id);
//            glActiveTexture(GL_TEXTURE0 + 1 - bias);
//            glBindTexture(GL_TEXTURE_2D, wood_box_specular_texture_id);
//            glActiveTexture(GL_TEXTURE0 + box_material.emission_texture);
//            glBindTexture(GL_TEXTURE_2D, emission_texture_id);
            // we must draw the "GL_FILL" first, otherwise the drawn border will be covered theoretically
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//            box_shader.set_bool("draw_border", false);
            // so what we are essentially doing is draw the same box, on different locations and rotations, for 10 times
            glDrawArrays(GL_TRIANGLES, 0, 36);
//            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//            box_shader.set_bool("draw_border", true);
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//            box_shader.set_bool("draw_border", false);
        }
        // draw ground, use the same shader as boxes
        ground_shader.use();
        glBindVertexArray(ground_VAO);
        {
            model_mat = glm::mat4(1.0f);
            model_mat = glm::translate(model_mat, glm::vec3(0.0f, -5.0f, 0.0f));
            {
                ground_shader.set_mat4("view_mat", view_mat);
                ground_shader.set_mat4("proj_mat", proj_mat);
                ground_shader.set_vec3("light.light_pos", light.light_pos);
                ground_shader.set_vec3("light.light_color", light.light_color);
                ground_shader.set_vec3("view_pos", camera.Position);
                ground_shader.set_mat4("model_mat", model_mat);
                ground_shader.set_vec3("spot_light.light_pos", spot_light.light_pos);
                ground_shader.set_vec3("spot_light.light_dir", spot_light.light_dir);
                ground_shader.set_vec3("spot_light.light_color", spot_light.light_color);
                ground_shader.set_vec3("directional_light.light_color", directional_light.light_color);
                ground_shader.set_vec3("light.light_color", light.light_color);
            }
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double width_pos, double height_pos) {
    if (first_mouse) {
        width_mouse = (float)width_pos;
        height_mouse = (float)height_pos;
        first_mouse = false;
    }
    float width_offset = (float)width_pos - width_mouse;
    float height_offset = height_mouse - (float)height_pos;
    width_mouse = (float)width_pos;
    height_mouse = (float)height_pos;
    camera.ProcessMouseMovement(width_offset, height_offset);
}

void scroll_callback(GLFWwindow* window, double unknown_offset, double scroll_offset) {
    camera.ProcessMouseScroll(scroll_offset);
}

unsigned int load_texture(const std::string& texture_path, unsigned int texture_unit) {
    int texture_width, texture_height, texture_channels;
    unsigned char* data = stbi_load(texture_path.c_str(), &texture_width,
                                    &texture_height, &texture_channels, 0);
    if (data == nullptr) {
        std::cout << "Fail to load texture" << std::endl;
        stbi_image_free(data);
        return -1;
    } else {
        std::cout << "Load texture successfully, channels=" << texture_channels << std::endl;
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLenum format;
        if (texture_channels == 1)
            format = GL_RED;
        else if (texture_channels == 3)
            format = GL_RGB;
        else if (texture_channels == 4)
            format = GL_RGBA;

        // repeat policy
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // interpolation policy
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // WARN: We should not set the Mag_Filter to be a mip-map filter, because for mag, we will never use mipmap
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture_width, texture_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return texture;
    }

}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        should_put_light = true;

}