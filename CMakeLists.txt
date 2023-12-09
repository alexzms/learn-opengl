cmake_minimum_required(VERSION 3.26)
project(learn_opengl_moving_triangle)
project(learn_texture)
project(learn_color)
project(learn_model)
project(learn_framebuffer)
project(learn_cubemap)
project(learn_geometry_shader)
project(learn_advanced_lighting)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(E:/ComputerGraphics/libraries/OpenGL/includes)
include_directories(E:/ComputerGraphics/libraries/Utilities/includes)
include_directories(includes)
link_directories(E:/ComputerGraphics/libraries/OpenGL/lib)
link_directories(E:/ComputerGraphics/libraries/Utilities/lib)

add_executable(learn_opengl_moving_triangle
        moving_triangle.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h)
add_executable(learn_texture
        texture_learning.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h)
add_executable(learn_color
        color_learning.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h
        includes/Model.h
        learn_framebuffer.cpp)

add_executable(learn_model
        learn_model.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h
        includes/Model.h)

add_executable(learn_framebuffer
        learn_framebuffer.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h
        includes/Model.h)

add_executable(learn_cubemap
        learn_cubemap.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h
        includes/Model.h)

add_executable(learn_geometry_shader
        learn_geometry_shader.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h
        includes/Model.h)

add_executable(learn_advanced_lighting
        learn_advanced_lighting.cpp
        lib/glad.c
        includes/PosData.h
        includes/Mesh.h
        includes/Model.h)

target_link_libraries(learn_opengl_moving_triangle glfw3.dll opengl32.lib)
target_link_libraries(learn_texture glfw3.dll opengl32.lib)
target_link_libraries(learn_color glfw3.dll opengl32.lib assimp)
target_link_libraries(learn_model glfw3.dll opengl32.lib assimp)
target_link_libraries(learn_framebuffer glfw3.dll opengl32.lib assimp)
target_link_libraries(learn_cubemap glfw3.dll opengl32.lib assimp)
target_link_libraries(learn_geometry_shader glfw3.dll opengl32.lib assimp)
target_link_libraries(learn_advanced_lighting glfw3.dll opengl32.lib assimp)
