#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "Shader.h"
#include "iostream"
#include "typeinfo"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void process_input(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* glfWwindow, int width, int height);
void mouse_callback(GLFWwindow* window, double width_pos, double height_pos);
void scroll_callback(GLFWwindow* window, double unknown_offset, double scroll_offset);
void test_glm_function();
bool genshin_start = false;
bool genshin_starting = false;
float delta_time = 0.0f;
float current_frame = 0.0f;
float last_frame = 0.0f;

bool first_mouse = true;
float sensitivity = 0.05;
float pitch = 0.0f;
float yaw = -90.0f;
float roll = 0.0f;
float mouse_width;
float mouse_height;

float fov = 45.0;

glm::vec3 camera_another_pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_another_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_another_up = glm::vec3(0.0f, 1.0f, 0.0f);


int main() {
    test_glm_function();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    stbi_set_flip_vertically_on_load(true);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    unsigned int RENDER_WIDTH = mode->width;
    unsigned int RENDER_HEIGHT = mode->height;
    mouse_width = RENDER_WIDTH / 2;
    mouse_height = RENDER_HEIGHT / 2;

    GLFWwindow *window = glfwCreateWindow(RENDER_WIDTH, RENDER_HEIGHT, "最伟大的作品",
                                          monitor, nullptr);
    if (window == nullptr) {
        std::cout << "Fail to create glf windows.." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Fail to init GLAD loader.." << std::endl;
        return -1;
    }
    glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // ----- Shader -----
    Shader shader("./shader/texture_learning.vert", "./shader/texture_learning.frag");
    shader.use();
    // ----- texture -----
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // blend policy
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // depth policy(enable depth buffer)
    glEnable(GL_DEPTH_TEST);
    // repeat policy
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // interpolation policy
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // WARN: We should not set the Mag_Filter to be a mip-map filter, because for mag, we will never use mipmap
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int texture_width, texture_height, texture_channals;
    int texture_width2, texture_height2, texture_channals2;
    unsigned char* data = stbi_load("./textures/genshin_sq.jpg", &texture_width,
                                    &texture_height, &texture_channals, 0);
    unsigned char* data2 = stbi_load("./textures/genshin_detail.jpg", &texture_width2,
                                     &texture_height2, &texture_channals2, 0);
    if (data == nullptr) {
        std::cout << "Fail to load texture" << std::endl;
        return -1;
    } else {
        std::cout << "Load texture successfully" << std::endl;
    }
    unsigned int texture[2];
    glGenTextures(2, &texture[0]);
    // before binding textures, we first activate the texture unit on gpu
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    //第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理
    //  （任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
    //第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
    //第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
    //第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
    //下个参数应该总是被设为0（历史遗留的问题）。
    //第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
    //最后一个参数是真正的图像数据。
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width2, texture_height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    // we've load the image into openGL, now we can free the image data
    stbi_image_free(data);

//    float genshin_vertices[] = {
//            //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
//            0.1875f,  0.33f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
//            0.1875f, -0.33f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
//            -0.1875f, -0.33f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
//            -0.1875f,  0.33f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
//    };
//    float genshin_vertices[] = {
//            //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
//            1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
//            1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
//            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
//            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
//    };
//    unsigned int indices[] = {
//            0, 1, 3, // first triangle
//            1, 2, 3  // second triangle
//    };
    float vertices[] = {
            // pos, texture_pos
            -0.1875f, -0.33f, -0.4f,  0.0f, 0.0f,
            0.1875f, -0.33f, -0.4f,  1.0f, 0.0f,
            0.1875f,  0.33f, -0.4f,  1.0f, 1.0f,
            0.1875f,  0.33f, -0.4f,  1.0f, 1.0f,
            -0.1875f,  0.33f, -0.4f,  0.0f, 1.0f,
            -0.1875f, -0.33f, -0.4f,  0.0f, 0.0f,

            -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f,
            0.1875f, -0.33f,  0.4f,  1.0f, 0.0f,
            0.1875f,  0.33f,  0.4f,  1.0f, 1.0f,
            0.1875f,  0.33f,  0.4f,  1.0f, 1.0f,
            -0.1875f,  0.33f,  0.4f,  0.0f, 1.0f,
            -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f,

            -0.1875f,  0.33f,  0.4f,  1.0f, 0.0f,
            -0.1875f,  0.33f, -0.4f,  1.0f, 1.0f,
            -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f,
            -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f,
            -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f,
            -0.1875f,  0.33f,  0.4f,  1.0f, 0.0f,

            0.1875f,  0.33f,  0.4,  1.0f, 0.0f,
            0.1875f,  0.33f, -0.4,  1.0f, 1.0f,
            0.1875f, -0.33f, -0.4,  0.0f, 1.0f,
            0.1875f, -0.33f, -0.4,  0.0f, 1.0f,
            0.1875f, -0.33f,  0.4,  0.0f, 0.0f,
            0.1875f,  0.33f,  0.4,  1.0f, 0.0f,

            -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f,
            0.1875f, -0.33f, -0.4f,  1.0f, 1.0f,
            0.1875f, -0.33f,  0.4f,  1.0f, 0.0f,
            0.1875f, -0.33f,  0.4f,  1.0f, 0.0f,
            -0.1875f, -0.33f,  0.4f,  0.0f, 0.0f,
            -0.1875f, -0.33f, -0.4f,  0.0f, 1.0f,

            -0.1875f,  0.33f, -0.4f,  0.0f, 1.0f,
            0.1875f,  0.33f, -0.4f,  1.0f, 1.0f,
            0.1875f,  0.33f,  0.4f,  1.0f, 0.0f,
            0.1875f,  0.33f,  0.4f,  1.0f, 0.0f,
            -0.1875f,  0.33f,  0.4f,  0.0f, 0.0f,
            -0.1875f,  0.33f, -0.4f,  0.0f, 1.0f
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
    // VAO, VBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    glGenBuffers(1, &EBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0); // pos
    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float))); // color
//    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float))); // tex
    glEnableVertexAttribArray(2);
    // camera vectors
    glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    // the reason we use neg, because instead of moving the camera, we move all objects in the world
    glm::vec3 camera_neg_front_axis = glm::normalize(camera_pos - camera_target);
    // to create the right-axis(x-axis) of the camera coordinate system, we need to create up vector first
    // and then cross-multiply by the camera_pos (image that will create a vector perpendicular to the
    // plane that's spanned by the camera_neg_front_axis and up vector
    glm::vec3 up_vec = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right_cam_axis = glm::normalize(glm::cross(up_vec, camera_neg_front_axis));
    // calculate up_camera_axis(y-axis) is just cross multiply the right-axis and camera_neg_front_axis
    glm::vec3 up_cam_axis = glm::cross(right_cam_axis, camera_neg_front_axis);
    // now we have camera_neg_front_axis, right_cam_axis, up_cam_axis

    // use glm::lookAt to directly create all vectors and arrange them in a matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(camera_pos,
                       camera_pos + camera_neg_front_axis,
                       up_cam_axis);

    // model matrix
    glm::mat4 model_mat = glm::mat4(1.0f);
    // rotate over x-axis of -55 degrees, to make it look like "laying" on the ground
    model_mat = glm::rotate(model_mat, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // view matrix(abandoned)
    glm::mat4 view_mat = glm::mat4(1.0f);
    // move every object by -3 on z-axis, so that the camera looks like moving by 3 on z-axis, which is backward
    view_mat = glm::translate(view_mat, glm::vec3(0.0f, 0.0f, -3.0f));
    // define projection matrix
    glm::mat4 ortho_mat = glm::ortho(0.0f, (float)RENDER_WIDTH,
                                     0.0f, (float)RENDER_HEIGHT,
                                     0.1f, 100.0f);
    glm::mat4 persp_mat = glm::perspective(glm::radians(fov),
                                           (float)RENDER_WIDTH/(float)RENDER_HEIGHT, 0.1f, 100.0f);

    // apply those matrices to the uniform variable in vertex shader
    {
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model_mat"), 1, GL_FALSE, glm::value_ptr(model_mat));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view_mat"), 1, GL_FALSE, glm::value_ptr(view_mat));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(persp_mat));
    }

    shader.set_float("alpha", 0.0f);
    shader.set_float("mix_value", 0.0f);
    shader.set_int("texture0", 0);
    shader.set_int("texture1", 1);
    float time = 0;
    float start_time = 0;
    float show_genshin_time = 1.0f;
    bool detail_press = false;
    bool start_detail = false;
    bool real_start_detail = false;
    float detail_time = 0;
    float radius = 10.0f;
    float camX, camY, camZ;
    camY = 0.0f;

    // render loop
    while(!glfwWindowShouldClose(window)) {
        process_input(window);

        current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;


        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        time = (float)glfwGetTime();
        if (genshin_start) {
            start_time = (float)glfwGetTime();
            genshin_start = false;
            genshin_starting = true;
            detail_press = false;
            start_detail = false;
            real_start_detail = false;
            std::cout << "Genshin, Qidong!" << std::endl;
            shader.set_float("mix_value", 0.0f);
        }
        if (genshin_starting) {
            float alpha = sin(time - start_time);
            if (alpha < 0.0f)
                alpha = 0.0f;
            if (alpha >= 0.99f) {
                alpha = 1.0f;
                genshin_starting = false;
                detail_time = (float)glfwGetTime();
//                start_detail = true;
            }
            shader.set_float("alpha", alpha);
        }
        if (start_detail) {
            if (time - detail_time >= 1) {
                detail_time = (float)glfwGetTime();
                start_detail = false;
                real_start_detail = true;
            }
        }
        if (real_start_detail) {
            float mix = sin(5 * (time - detail_time));
            if (mix < 0.0f)
                mix = 0.0f;
            if (mix >= 0.99f) {
                mix = 1.0f;
                real_start_detail = false;
            }
            shader.set_float("mix_value", mix);
        }
//        // transformation matrix
//        glm::mat4 trans = glm::mat4(1.0f);
//        // rotate by 90 degrees on z axis
//        trans = glm::rotate(trans, glm::radians(10 * ((float)glfwGetTime() - start_time)),
//                            glm::vec3(0.0f, 0.0f, 1.0f));
        // scale by 0.5 on all axis
//        trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));
        // set the uniform parameter to trans
//        {
//            unsigned int transformLoc = glGetUniformLocation(shader.ID, "trans");
//            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
//        }
        camX = sin((float)glfwGetTime()) * radius;
        camZ = cos((float)glfwGetTime()) * radius;

        view = glm::lookAt(camera_another_pos,
                           camera_another_pos + camera_another_front,
                           camera_another_up);
        persp_mat = glm::perspective(glm::radians(fov),
                                               (float)RENDER_WIDTH/(float)RENDER_HEIGHT, 0.1f, 100.0f);
        {
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view_mat"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "proj_mat"), 1, GL_FALSE, glm::value_ptr(persp_mat));
        }
        glBindVertexArray(VAO);
        for (int i = 0; i != 10; ++i) {
            model_mat = glm::mat4(1.0f);
            model_mat = glm::translate(model_mat, cubePositions[i]);
            model_mat = glm::scale(model_mat, glm::vec3(1.2f, 1.2f, 1.2f));
            model_mat = glm::rotate(model_mat,
                                    30.0f * (float)glfwGetTime() * glm::radians((float)(i + 1)),
                                    glm::vec3(sin(i), cos(i), tan(i)));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model_mat"), 1, GL_FALSE, glm::value_ptr(model_mat));

            // we must draw the "GL_FILL" first, otherwise the drawn border will be covered theoretically
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            shader.set_bool("draw_border", false);
            // so what we are essentially doing is draw the same box, on different locations and rotations, for 10 times
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            shader.set_bool("draw_border", true);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            shader.set_bool("draw_border", false);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *glfWwindow, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double width_pos, double height_pos) {
    if (first_mouse) {
        mouse_width = (float)width_pos;
        mouse_height = (float)height_pos;
        first_mouse = false;
    }
    float width_offset = (float)width_pos - mouse_width;
    float height_offset = mouse_height - (float)height_pos;
    mouse_width = (float)width_pos;
    mouse_height = (float)height_pos;
    width_offset *= sensitivity;
    height_offset *= sensitivity;

    yaw += width_offset;
    pitch += height_offset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
//    std::cout << "roll: " << roll << " pitch: " << pitch << " yaw: " << yaw << std::endl;
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_another_front = glm::normalize(front);
//    std:: cout << "camera_another_front: " << camera_another_front.x << " " << camera_another_front.y << " " << camera_another_front.z << std::endl;
}

void process_input(GLFWwindow* window) {
    float camera_speed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        genshin_start = true;
        genshin_starting = false;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_another_pos += camera_speed * camera_another_front;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_another_pos -= camera_speed * camera_another_front;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_another_pos -= camera_speed * glm::normalize(glm::cross(camera_another_front, camera_another_up));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_another_pos += camera_speed * glm::normalize(glm::cross(camera_another_front, camera_another_up));
    }
}

void scroll_callback(GLFWwindow* window, double unknown_offset, double scroll_offset) {
    fov -= scroll_offset;
    if (fov <= 1.0f)
        fov = 1.0f;
}

void test_glm_function() {
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    // Builds a translation 4 * 4 matrix created from a vector vec_t of 3 components.
    // So that for any vec0, trans * vec0 = vec0 + vec_t
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    std::cout << "trans type: " << typeid(trans).name() << std::endl;
    vec = trans * vec;
    std::cout << "vec value: " << vec.x << " " << vec.y << " " << vec.z << std::endl;
}