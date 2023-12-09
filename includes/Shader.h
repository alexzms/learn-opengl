//
// Created by alexzms on 2023/10/1.
//

#ifndef LEARN_OPENGL_CLION_SHADER_H
#define LEARN_OPENGL_CLION_SHADER_H

#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.h"

#include "string"
#include "iostream"
#include "fstream"
#include "sstream"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Shader{
public:
    // shader ID
    unsigned int ID;
    bool functional = false;

    Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);

    // activate program
    void use() const;
    // uniform value set
    void set_bool(const std::string &name, bool value) const;
    void set_int(const std::string &name, int value) const;
    void set_float(const std::string &name, float value) const;
    void set_vec3(const std::string &name, float x, float y, float z) const;
    void set_vec3(const std::string &name, glm::vec3 vec) const;
    void set_mat4(const std::string &name, glm::mat4 mat) const;
};

Shader::Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path) {
    bool set_functional = true;
    std::string vertex_code;
    std::string fragment_code;
    std::string geometry_code;

    std::ifstream vertex_file;
    std::ifstream fragment_file;
    std::ifstream geometry_file;
    vertex_file.exceptions(std::ifstream::failbit | std::ifstream ::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream ::badbit);
    if (geometry_path != nullptr) {
        geometry_file.exceptions(std::ifstream::failbit | std::ifstream ::badbit);
    }

    try {
        vertex_file.open(vertex_path);
        fragment_file.open(fragment_path);

        std::stringstream vertex_stream, fragment_stream;
        vertex_stream << vertex_file.rdbuf();
        fragment_stream << fragment_file.rdbuf();

        vertex_file.close();
        fragment_file.close();

        vertex_code = vertex_stream.str();
        fragment_code = fragment_stream.str();
        if (geometry_path != nullptr) {
            geometry_file.open(geometry_path);
            std::stringstream geometry_stream;
            geometry_stream << geometry_file.rdbuf();
            geometry_file.close();
            geometry_code = geometry_stream.str();
        }
    } catch (std::ifstream ::failure &e) {
        std::cout << "ERROR::Shader::File not read successfully" << std::endl;
    }

    const char* vertex_shader_code = vertex_code.c_str();
    const char* fragment_shader_code = fragment_code.c_str();
    const char* geometry_shader_code = geometry_code.c_str();

    unsigned int vertex_shader, fragment_shader, geometry_shader;
    int success;
    char info_log[512];

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        set_functional = false;
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        std::cout << "ERROR::Shader::Vertex shader compile failed\n" << info_log << std::endl;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        set_functional = false;
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        std::cout << "ERROR::Shader::Fragment shader compile failed\n" << info_log << std::endl;
    }

    if (geometry_path != nullptr) {
        geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry_shader, 1, &geometry_shader_code, nullptr);
        glCompileShader(geometry_shader);
        glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            set_functional = false;
            glGetShaderInfoLog(geometry_shader, 512, nullptr, info_log);
            std::cout << "ERROR::Shader::Geometry shader compile failed\n" << info_log << std::endl;
        }
    }

    Shader::ID = glCreateProgram();
    glAttachShader(ID, vertex_shader);
    glAttachShader(ID, fragment_shader);
    if (geometry_path != nullptr) {
        glAttachShader(ID, geometry_shader);
    }
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        set_functional = false;
        glGetProgramInfoLog(ID, 512, nullptr, info_log);
        std::cout << "ERROR::Shader::Shader program link failed\n" << info_log << std::endl;
    }

    Shader::functional = set_functional;
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    if (geometry_path != nullptr) {
        glDeleteShader(geometry_shader);
    }
}

void Shader::use() const{
    glUseProgram(Shader::ID);
}

void Shader::set_bool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
}

void Shader::set_int(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_vec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::set_vec3(const std::string &name, glm::vec3 vec) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::set_mat4(const std::string &name, glm::mat4 mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

#endif //LEARN_OPENGL_CLION_SHADER_H
