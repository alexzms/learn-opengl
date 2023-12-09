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

int first_steps() {
    // init glfw
    glfwInit();
    // set glfw version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // set glfw profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn Geometry", nullptr, nullptr);
    // check if window is created
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        // terminate glfw
        glfwTerminate();
        return -1;
    }
    // set window to current context
    glfwMakeContextCurrent(window);
    // init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }
    // set viewport
    glViewport(0, 0, 800, 600);
    // set window resize callback
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });
    // set window close callback
    glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
        glfwSetWindowShouldClose(window, true);
    });

    Shader shader("./shader/geometry_shader/first_steps.vert",
                  "./shader/geometry_shader/first_steps.frag",
                  "./shader/geometry_shader/first_steps.geom");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float points[] = {
            -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 左上
            0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // 右上
            0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 右下
            -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // 左下
    };
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    // just draw points, so it's simple
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        // draw triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);
        // swap buffer and poll event
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

int main() {
    first_steps();
    return 0;
}