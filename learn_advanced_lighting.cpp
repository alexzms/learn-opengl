#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "iostream"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/Importer.hpp"
#include "includes/PosData.h"
#include "includes/Light_Material.h"
#include "Camera.h"
#include "Model.h"
#include "vector"
#include "string"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double width_pos, double height_pos);
void scroll_callback(GLFWwindow* window, double unknown_offset, double scroll_offset);
unsigned int load_texture(const std::string& texture_path, unsigned int texture_unit);
int create_framebuffer(float scale, GLsizei FRAMEBUFFER_WIDTH, GLsizei FRAMEBUFFER_HEIGHT,
                       unsigned int* FBO,
                       unsigned int* texture_color_framebuffer,
                       unsigned int* renderbuffer_depth_stencil_framebuffer);
int shaders_set_lighting(Shader* shaders, unsigned int shader_len, Light* light, SpotLight* spot_light,
                         DirectionalLight* directional_light);
int generate_cube_texture(unsigned int *texture_id, const std::vector<std::string>& texture_paths);


static Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float prev_time = 0.0f;
float delta_time = 0.0f;
float width_mouse = 0.0f;
float height_mouse = 0.0f;
bool first_mouse = true;
bool should_put_light = false;
glm::vec3 light_init_pos = glm::vec3(1.2f, 1.0f, 2.0f);
float overall_light_enhance = 1.0f;


int main() {
    generate_glass_info(glm::vec3(0.0f, 10.0f, 0.0f));

    // glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    stbi_set_flip_vertically_on_load(true);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    const GLsizei RENDER_WIDTH = mode->width;
    const GLsizei RENDER_HEIGHT = mode->height;
    GLFWwindow* window = glfwCreateWindow(RENDER_WIDTH, RENDER_HEIGHT,
                                          "Learn_Model", monitor, nullptr);
    if (window == nullptr) {
        std::cout << "[FATAL]Fail to create glfw window.." << std::endl;
        glfwTerminate();
        return -1;
    } else {
        std::cout << "[INFO]Create glfw window successfully.." << std::endl;
    }
    glfwMakeContextCurrent(window);
    // glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "[FATAL]Fail to init GLAD loader.." << std::endl;
        glfwTerminate();
        return -1;
    } else {
        std::cout << "[INFO]Init GLAD loader successfully.." << std::endl;
    }

    glViewport(0, 0, (int)RENDER_WIDTH, (int)RENDER_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    std::cout << "[INFO]OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // blend policy
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // default
    glm::mat4 proj_mat = glm::mat4(1.0f);
    // shader
    Shader object_shader("./shader/blinn_phong/blinn_phong_model.vert",
                         "./shader/blinn_phong/blinn_phong_model.frag");
    Shader light_shader("shader/color_learning/light.vert",
                        "shader/color_learning/light.frag");
    Shader plane_shader("./shader/blinn_phong/blinn_phong_plane.vert",
                        "./shader/blinn_phong/blinn_phong_plane.frag");
    Shader border_shader("./shader/learn_model/border.vert",
                         "./shader/learn_model/border.frag");
    Model our_model("objects/nanosuit/nanosuit.obj");
    unsigned int ground_texture_id = load_texture("./textures/awesomeface.png", 3);
    unsigned int grass_texture_id = load_texture("./textures/grass.png", 4);
    unsigned int glass_texture_id = load_texture("./textures/blending_transparent_window.png", 5);
    Light light{};
    {
        light.light_color = glm::vec3(1.0f, 1.0f, 1.0f);
        light.light_pos = light_init_pos;
        float enhance = 0.3;
        light.ambient = enhance * glm::vec3(0.4f, 0.4f, 0.4f);
        light.diffuse = enhance * glm::vec3(2.5f, 2.5f, 2.5f);
        light.specular = enhance * 1.3f * glm::vec3(2.9f, 2.9f, 2.9f);
        light.constant = 1.0f;
        light.linear = 0.09f;
        light.quadratic = 0.032f;
    }

    SpotLight spot_light{};
    {
        float enhance_spot_light = 0.3;
        spot_light.light_color = glm::vec3(1.0f, 1.0f, 1.0f);
        spot_light.light_pos = camera.Position;
        spot_light.light_dir = camera.Front;
        spot_light.ambient = enhance_spot_light * glm::vec3(0.0f, 0.0f, 0.0f);
        spot_light.diffuse = enhance_spot_light * glm::vec3(2.5f, 2.5f, 2.5f);
        spot_light.specular = enhance_spot_light * 1.3f * glm::vec3(2.9f, 2.9f, 2.9f);
        spot_light.cut_off = glm::cos(glm::radians(12.5f));
        spot_light.outer_cut_off = glm::cos(glm::radians(17.5f));
        spot_light.constant = 1.0f;
        spot_light.linear = 0.09f;
        spot_light.quadratic = 0.032f;
    }

    DirectionalLight directional_light{};
    {
        float enhance_directional_light = 0.2;
        directional_light.light_color = glm::vec3(1.0f, 1.0f, 1.0f);
        directional_light.direction = glm::vec3(0.5f, -0.7f, 3.0f);
        directional_light.ambient = enhance_directional_light * glm::vec3(0.4f, 0.4f, 0.4f);
        directional_light.diffuse = enhance_directional_light * glm::vec3(2.5f, 2.5f, 2.5f);
        directional_light.specular = enhance_directional_light * 1.3f * glm::vec3(2.9f, 2.9f, 2.9f);
    }

    Material ground_material{};
    {
        ground_material.diffuse_texture = 3;
        ground_material.specular_texture = 3;
        ground_material.emission_texture = 3;
        ground_material.shininess = 16.0f;
    }

    Material grass_material{};
    {
        grass_material.diffuse_texture = 4;
        grass_material.specular_texture = 4;
        grass_material.emission_texture = 4;
        grass_material.shininess = 16.0f;
    }

    Material glass_material{};
    {
        glass_material.diffuse_texture = 5;
        glass_material.specular_texture = 5;
        glass_material.emission_texture = 5;
        glass_material.shininess = 256.0f;
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

    unsigned int light_VAO, boxes_VBO;
    {
        glGenVertexArrays(1, &light_VAO);
        glBindVertexArray(light_VAO);
        glGenBuffers(1, &boxes_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, boxes_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wood_box_vertices), wood_box_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0); // pos is all we need
        glEnableVertexAttribArray(0);
    }

    shaders_set_lighting(&object_shader, 1, &light, &spot_light, &directional_light);
    shaders_set_lighting(&plane_shader, 1, &light, &spot_light, &directional_light);

    // framebuffer, for post-processing
    float scale = 1.0f;
    const auto FRAMEBUFFER_WIDTH = (GLsizei)((float)RENDER_WIDTH / scale);
    const auto FRAMEBUFFER_HEIGHT = (GLsizei)((float)RENDER_HEIGHT / scale);
    auto *texture_color_framebuffer = new unsigned int;
    auto *renderbuffer_depth_stencil_framebuffer = new unsigned int;
    auto *FBO = new unsigned int;
    create_framebuffer(scale, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, FBO,
                       texture_color_framebuffer,
                       renderbuffer_depth_stencil_framebuffer);
    // shader for render framebuffer
    Shader framebuffer_shader("./shader/learn_framebuffer/screen_plane.vert",
                              "./shader/learn_framebuffer/screen_plane.frag");
    unsigned int framebuffer_VAO, framebuffer_VBO, framebuffer_texture_gpu_id = 0;
    {
        glGenVertexArrays(1, &framebuffer_VAO);
        glBindVertexArray(framebuffer_VAO);
        glGenBuffers(1, &framebuffer_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, framebuffer_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(screen_square_vertices), screen_square_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    framebuffer_shader.use();
    framebuffer_shader.set_int("tex", (int)framebuffer_texture_gpu_id);
    framebuffer_shader.set_int("screen_width", RENDER_WIDTH);
    framebuffer_shader.set_int("screen_height", RENDER_HEIGHT);

    // create skybox
    auto *skybox_texture_id = new unsigned int;
    generate_cube_texture(skybox_texture_id, skybox_texture_paths);
    Shader skybox_shader(
            "./shader/learn_cubemap/cubemap.vert", "shader/learn_cubemap/cubemap.frag"
    );
    unsigned int skybox_VAO, skybox_VBO, skybox_texture_gpu_id;
    {
        skybox_texture_gpu_id = 0;
        glGenVertexArrays(1, &skybox_VAO);
        glBindVertexArray(skybox_VAO);
        glGenBuffers(1, &skybox_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
    skybox_shader.use();
    skybox_shader.set_int("cube_texture", skybox_texture_gpu_id);
    plane_shader.set_int("skybox_texture", skybox_texture_gpu_id);

    // polygon mode
//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // render loop
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    while (!glfwWindowShouldClose(window)) {
        auto current_time = (float)glfwGetTime();
        delta_time = current_time - prev_time;
        prev_time = current_time;
        process_input(window);
        generate_glass_info(camera.Position);

        proj_mat = glm::perspective(glm::radians(camera.Zoom),
                                    (float)RENDER_WIDTH / (float) RENDER_HEIGHT,
                                    0.1f, 100.0f);

        // render start
        glBindFramebuffer(GL_FRAMEBUFFER, *FBO);  // render to FBO, not default screen
        glViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // render skybox, first we need to disable depth test, so that the skybox will not block other things
        glDepthMask(GL_FALSE);
        skybox_shader.use();
        skybox_shader.set_mat4("proj_mat", proj_mat);
        skybox_shader.set_mat4("view_mat", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        glActiveTexture(GL_TEXTURE0 + skybox_texture_gpu_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *skybox_texture_id);
        glBindVertexArray(skybox_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // depth policy(enable depth buffer)
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        // keep the stencil buffer value unchanged if stencil test fail, and set it to ref value if stencil test pass
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // when drawing the light cube, we will set all the stencil value of fragment of the cube to be 1
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF); // enable writing to stencil buffer
        light_shader.use();
        spot_light.light_pos = camera.Position;
        spot_light.light_dir = camera.Front;
        if (should_put_light){
            light.light_pos = camera.Position + 2.0f * camera.Front;
            should_put_light = false;
        }
        {
            glm::mat4 view_mat = camera.GetViewMatrix();
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
        }
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // we wont overwrite the already-drawn light cube
        glStencilMask(0x00); // disable writing to stencil buffer
        border_shader.use();
        {
            glm::mat4 view_mat = camera.GetViewMatrix();
            {
                border_shader.set_mat4("view_mat", view_mat);
                border_shader.set_mat4("proj_mat", proj_mat);
                border_shader.set_mat4("model_mat",
                                       glm::scale(glm::translate(glm::mat4(1.0f), light.light_pos),
                                                  glm::vec3(0.25f, 0.25f, 0.25f)));
            }
            glBindVertexArray(light_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glStencilMask(0xFF); // enable writing to stencil buffer
        glDisable(GL_STENCIL_TEST);

        // draw model
        object_shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        {
            object_shader.set_mat4("model_mat", model);
            object_shader.set_mat4("view_mat", camera.GetViewMatrix());
            object_shader.set_mat4("proj_mat", proj_mat);
            object_shader.set_vec3("light.light_pos", light.light_pos);
            object_shader.set_vec3("light.light_color", light.light_color);
            object_shader.set_vec3("spot_light.light_color", spot_light.light_color);
            object_shader.set_vec3("directional_light.light_color", directional_light.light_color);
            object_shader.set_vec3("spot_light.light_pos", spot_light.light_pos);
            object_shader.set_vec3("spot_light.light_dir", spot_light.light_dir);
            object_shader.set_vec3("view_pos", camera.Position);
            object_shader.set_float("overall_light_enhance", overall_light_enhance);
        }
        our_model.Draw(object_shader);

        // draw ground
        plane_shader.use();
        glBindVertexArray(ground_VAO);
        glActiveTexture(GL_TEXTURE0 + ground_material.diffuse_texture);
        glBindTexture(GL_TEXTURE_2D, ground_texture_id);
        glActiveTexture(GL_TEXTURE0 + ground_material.specular_texture);
        glBindTexture(GL_TEXTURE_2D, ground_texture_id);
        glActiveTexture(GL_TEXTURE0 + skybox_texture_gpu_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *skybox_texture_id);
//        glActiveTexture(GL_TEXTURE0 + ground_material.emission_texture);
//        glBindTexture(GL_TEXTURE_2D, ground_texture_id);
        {
            glm::mat4 model_mat = glm::mat4(1.0f);
            model_mat = glm::translate(model_mat, glm::vec3(0.0f, -5.0f, 0.0f));
            // scale up 25 times
            model_mat = glm::scale(model_mat, glm::vec3(25.0f, 25.0f, 25.0f));
            {
                plane_shader.set_int("material.diffuse_texture", ground_material.diffuse_texture);
                plane_shader.set_int("material.specular_texture", ground_material.specular_texture);
//                plane_shader.set_int("material.emission_texture", ground_material.emission_texture);
                plane_shader.set_float("material.shininess", ground_material.shininess);
                plane_shader.set_mat4("view_mat", camera.GetViewMatrix());
                plane_shader.set_mat4("proj_mat", proj_mat);
                plane_shader.set_vec3("light.light_pos", light.light_pos);
                plane_shader.set_vec3("light.light_color", light.light_color);
                plane_shader.set_vec3("view_pos", camera.Position);
                plane_shader.set_mat4("model_mat", model_mat);
                plane_shader.set_vec3("spot_light.light_pos", spot_light.light_pos);
                plane_shader.set_vec3("spot_light.light_dir", spot_light.light_dir);
                plane_shader.set_vec3("spot_light.light_color", spot_light.light_color);
                plane_shader.set_vec3("directional_light.light_color", directional_light.light_color);
                plane_shader.set_vec3("light.light_color", light.light_color);
                plane_shader.set_float("skybox_reflect_coeff", 0.0f);
            }
//            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // draw vegetation, use the same VAO as ground, since it's also a plane
        glActiveTexture(GL_TEXTURE0 + grass_material.diffuse_texture);
        glBindTexture(GL_TEXTURE_2D, grass_texture_id);
        glActiveTexture(GL_TEXTURE0 + grass_material.specular_texture);
        glBindTexture(GL_TEXTURE_2D, grass_texture_id);
        glActiveTexture(GL_TEXTURE0 + skybox_texture_gpu_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *skybox_texture_id);
        {
            plane_shader.set_vec3("light.light_pos", light.light_pos);
            plane_shader.set_vec3("light.light_color", light.light_color);
            plane_shader.set_vec3("view_pos", camera.Position);
            plane_shader.set_vec3("spot_light.light_pos", spot_light.light_pos);
            plane_shader.set_vec3("spot_light.light_dir", spot_light.light_dir);
            plane_shader.set_vec3("spot_light.light_color", spot_light.light_color);
            plane_shader.set_vec3("directional_light.light_color", directional_light.light_color);
            plane_shader.set_vec3("light.light_color", light.light_color);
            plane_shader.set_float("skybox_reflect_coeff", 0.0f);
        }
        // no more emission texture needed, loop over vegetation_pos
        {
            plane_shader.set_int("material.diffuse_texture", grass_material.diffuse_texture);
            plane_shader.set_int("material.specular_texture", grass_material.specular_texture);
            plane_shader.set_float("material.shininess", grass_material.shininess);
            plane_shader.set_mat4("view_mat", camera.GetViewMatrix());
            plane_shader.set_mat4("proj_mat", proj_mat);
        }
        for (auto vegetation_po : vegetation_pos) {
            glm::mat4 model_mat = glm::mat4(1.0f);
            model_mat = glm::translate(model_mat, vegetation_po);
            // rotate the vegetation 90 degrees around the x-axis so it stands up
            model_mat = glm::rotate(model_mat, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0));
            {
                plane_shader.set_mat4("model_mat", model_mat);
            }
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

//         draw glass
        glActiveTexture(GL_TEXTURE0 + glass_material.diffuse_texture);
        glBindTexture(GL_TEXTURE_2D, glass_texture_id);
        glActiveTexture(GL_TEXTURE0 + glass_material.specular_texture);
        glBindTexture(GL_TEXTURE_2D, glass_texture_id);
        glActiveTexture(GL_TEXTURE0 + skybox_texture_gpu_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, *skybox_texture_id);
        // no more emission texture needed, spawn 100 glass plane randomly
        // sort the glass plane by distance to camera
        std::map<float, int > sorted;
        for (int i = 0; i < GLASS_NUM; ++i) {
            float distance = glm::length(camera.Position - glass_pos[i]);
            sorted[distance] = i;
        }
        {
            plane_shader.set_int("material.diffuse_texture", glass_material.diffuse_texture);
            plane_shader.set_int("material.specular_texture", glass_material.specular_texture);
            plane_shader.set_float("material.shininess", glass_material.shininess);
            plane_shader.set_mat4("view_mat", camera.GetViewMatrix());
            plane_shader.set_mat4("proj_mat", proj_mat);
            plane_shader.set_float("skybox_reflect_coeff", 1.0f);
        }
        for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
            glm::mat4 model_mat = glm::mat4(1.0f);
            model_mat = glm::translate(model_mat, glass_pos[it->second]);
            // scale
            model_mat = glm::scale(model_mat, glass_scale[it->second]);
            // rotate
            model_mat = glm::rotate(model_mat, glm::radians(glass_rotate[it->second].x),
                                    glm::vec3(1.0f, 0.0f, 0.0f));
            model_mat = glm::rotate(model_mat, glm::radians(glass_rotate[it->second].y),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
            model_mat = glm::rotate(model_mat, glm::radians(glass_rotate[it->second].z),
                                    glm::vec3(0.0f, 0.0f, 1.0f));
            {
                plane_shader.set_mat4("model_mat", model_mat);
            }
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }


        // render the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  // set back to default framebuffer
        glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
        glDisable(GL_DEPTH_TEST);  // disable because we don't need any depth test while drawing a plane for screen
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        framebuffer_shader.use();
        glBindVertexArray(framebuffer_VAO);
        glActiveTexture(GL_TEXTURE0 + framebuffer_texture_gpu_id);
        glBindTexture(GL_TEXTURE_2D, *texture_color_framebuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    delete texture_color_framebuffer;
    delete renderbuffer_depth_stencil_framebuffer;
    delete FBO;
    delete skybox_texture_id;
    return 0;
}

int generate_cube_texture(unsigned int *texture_id, const std::vector<std::string>& texture_paths) {
    glGenTextures(1, texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *texture_id);
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data;
    int width, height, channel;
    for (int i = 0; i != texture_paths.size(); ++i) {
        data = stbi_load(texture_paths[i].c_str(), &width, &height, &channel, 0);
        if (data == nullptr) {
            std::cout << "[ERR] generate_cube_texture: not able to load texture " << i << std::endl;
            stbi_image_free(data);
            return -1;
        }
        GLint format;
        GLint internal_format;
        if (channel == 1) {
            format = GL_RED;
            internal_format = GL_RED;
        }
        else if (channel == 3) {
            format = GL_RGB;
            internal_format = GL_SRGB;
        }
        else if (channel == 4) {
            format = GL_RGBA;
            internal_format = GL_SRGB_ALPHA;
        }
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // right, left, up, down, front, back
                0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data
        );
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    stbi_set_flip_vertically_on_load(true);
    return 0;
}

int shaders_set_lighting(Shader* shaders, unsigned int shader_len, Light* light, SpotLight* spot_light,
                         DirectionalLight* directional_light) {
    for (int i = 0; i < shader_len; ++i) {
        Shader object_shader = shaders[i];
        object_shader.use();
        object_shader.set_vec3("light.ambient", light->ambient);
        object_shader.set_vec3("light.diffuse", light->diffuse);
        object_shader.set_vec3("light.specular", light->specular);
        object_shader.set_float("light.constant", light->constant);
        object_shader.set_float("light.linear", light->linear);
        object_shader.set_float("light.quadratic", light->quadratic);
        object_shader.set_vec3("spot_light.ambient", spot_light->ambient);
        object_shader.set_vec3("spot_light.diffuse", spot_light->diffuse);
        object_shader.set_vec3("spot_light.specular", spot_light->specular);
        object_shader.set_float("spot_light.cut_off", spot_light->cut_off);
        object_shader.set_float("spot_light.outer_cut_off", spot_light->outer_cut_off);
        object_shader.set_float("spot_light.constant", spot_light->constant);
        object_shader.set_float("spot_light.linear", spot_light->linear);
        object_shader.set_float("spot_light.quadratic", spot_light->quadratic);
        object_shader.set_vec3("directional_light.light_color", directional_light->light_color);
        object_shader.set_vec3("directional_light.direction", directional_light->direction);
        object_shader.set_vec3("directional_light.ambient", directional_light->ambient);
        object_shader.set_vec3("directional_light.diffuse", directional_light->diffuse);
        object_shader.set_vec3("directional_light.specular", directional_light->specular);
    }
    return 0;
}

int create_framebuffer(float scale, GLsizei FRAMEBUFFER_WIDTH, GLsizei FRAMEBUFFER_HEIGHT,
                       unsigned int* FBO,
                       unsigned int* texture_color_framebuffer,
                       unsigned int* renderbuffer_depth_stencil_framebuffer) {
    glGenFramebuffers(1, FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, *FBO);
    // texture for attachment
    glGenTextures(1, texture_color_framebuffer);
    glBindTexture(GL_TEXTURE_2D, *texture_color_framebuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // GL_REPEAT
    // attach the color texture to framebuffer we've created
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture_color_framebuffer, 0);
    // create a renderbuffer object for depth and stencil attachment
    // 渲染缓冲对象能为你的帧缓冲对象提供一些优化，但知道什么时候使用渲染缓冲对象，什么时候使用纹理是很重要的。
    // 通常的规则是，如果你不需要从一个缓冲中采样数据，那么对这个缓冲使用渲染缓冲对象会是明智的选择。
    // 如果你需要从缓冲中采样颜色或深度值等数据，那么你应该选择纹理附件。性能方面它不会产生非常大的影响的。
    glGenRenderbuffers(1, renderbuffer_depth_stencil_framebuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *renderbuffer_depth_stencil_framebuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);  // 24bit d, 8bit s
    // attach the renderbuffer object to depth and stencil attachment
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, *renderbuffer_depth_stencil_framebuffer);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "[WARN]Frame Buffer Generation is incomplete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  // reset to default framebuffer
        return -1;
    } else {
        std::cout << "[INFO]Frame Buffer Generation is complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);  // reset to default framebuffer
        return 0;
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
    camera.ProcessMouseScroll((float)scroll_offset);
}

unsigned int load_texture(const std::string& texture_path, unsigned int texture_unit) {
    int texture_width, texture_height, texture_channels;
    unsigned char* data = stbi_load(texture_path.c_str(), &texture_width,
                                    &texture_height, &texture_channels, 0);
    if (data == nullptr) {
        std::cout << "[FATAL]Fail to load texture" << std::endl;
        stbi_image_free(data);
        return -1;
    } else {
        std::cout << "[INFO]Load texture successfully, channels=" << texture_channels << std::endl;
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        GLint format;
        GLint internal_format;
        if (texture_channels == 1) {
            format = GL_RED;
            internal_format = GL_RED;
        }
        else if (texture_channels == 3) {
            format = GL_RGB;
            internal_format = GL_SRGB;
        }
        else if (texture_channels == 4) {
            format = GL_RGBA;
            internal_format = GL_SRGB_ALPHA;
        }

        // repeat policy
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // interpolation policy
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // WARN: We should not set the Mag_Filter to be a mip-map filter, because for mag, we will never use mipmap
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture_width, texture_height,
                     0, format, GL_UNSIGNED_BYTE, data);
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
    // q, e: light enhance
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        overall_light_enhance += delta_time;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        overall_light_enhance -= delta_time;
    // r to regenerate glass
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        generate_glass_info(camera.Position);
}
