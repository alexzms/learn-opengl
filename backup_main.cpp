#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //no border window
    //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Learn_OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Fail to create glf windows.." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Fail to init GLAD loader.." << std::endl;
        return -1;
    }
    // check for allowed number of vertex attribute, and it should be after glad is load
    int num_vertex_attribute;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &num_vertex_attribute);
    std::cout << "Max supportted number of vertex attributes: " << num_vertex_attribute << std::endl;

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLfloat increment = 0.0f;
    int sign = 1;


    // vertex shader
    const char* vertex_shader_source = "#version 460 core\n"
                                       "layout (location = 0) in vec3 aPos;\n"
                                       "out vec4 vertex_color;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   gl_Position = vec4(aPos, 1.0);\n"
                                       "   vertex_color = vec4(0.1f, 0.9f, 0.2f, 1.0f);\n"
                                       "}\0";
    unsigned int vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);
    {
        // vertex shader compile status
        int success;
        char compile_log[512];
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(vertex_shader, 512, nullptr, compile_log);
        std::cout << "Vertex shader compile status: " << (success ? "true" : "false") << std::endl;
        std::cout << "Vertex shader compile log: " << compile_log << std::endl;
    }
    // fragment shader
    unsigned int fragment_shader;
    const char* fragment_shader_source = "#version 460 core\n"
                                         "in vec4 vertex_color;\n"
                                         "out vec4 FragColor;\n"
                                         "uniform vec4 our_color;\n"
                                         "\n"
                                         "void main()\n"
                                         "{\n"
                                         "    FragColor = our_color;\n"
                                         "} ";
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);
    {
        // fragment shader compile status
        int success;
        char compile_log[512];
        glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
        glGetShaderInfoLog(fragment_shader, 512, nullptr, compile_log);
        std::cout << "Fragment shader compile status: " << (success ? "true" : "false") << std::endl;
        std::cout << "Fragment shader compile log: " << compile_log << std::endl;
    }
    // shader program object
    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    {
        // shader program link status
        int success;
        char link_log[512];
        glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(shader_program, 512, nullptr, link_log);
        std::cout << "Shader program link status: " << (success ? "true" : "false") << std::endl;
        std::cout << "Shader program link compile log: " << link_log << std::endl;
    }
    // we no longer need those shaders since we've linked them to the program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    {
        // this is not good, because we re-rendered some of the vertices! this is time-consuming
//        float vertices[] = {
//                // 第一个三角形
//                0.5f, 0.5f, 0.0f,   // 右上角
//                0.5f, -0.5f, 0.0f,  // 右下角
//                -0.5f, 0.5f, 0.0f,  // 左上角
//                // 第二个三角形
//                0.5f, -0.5f, 0.0f,  // 右下角
//                -0.5f, -0.5f, 0.0f, // 左下角
//                -0.5f, 0.5f, 0.0f   // 左上角
//        };
    }
    // we should use this
    float vertices[] = {
            0.5f, 0.5f, 0.0f,   // 右上角
            0.5f, -0.5f, 0.0f,  // 右下角
            -0.5f, -0.5f, 0.0f, // 左下角
            -0.5f, 0.5f, 0.0f   // 左上角
    };
    // element buffer object, use indices to reduce the repeating vertices
    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    // always get VAO first
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // and then VBO
    // vertex buffer objects to manage memory in gpu for vertices[]
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set and send buffer data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // finally EBO
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);

    // vertex attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // get the uniform our_color
    int our_color_location = glGetUniformLocation(shader_program, "our_color");

    // line-draw mode
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(increment, 1.0f - increment, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        increment += (float)sign * 0.001f;
        if (increment >= 1.0f || increment <= 0)
            sign = -sign;

        // render stuff
        // ...
        // activate the shader_program
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glUniform4f(our_color_location, 1.0f - increment, increment, 0.3f, 1.0f);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // release binding
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}