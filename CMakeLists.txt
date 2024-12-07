set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

cmake_minimum_required(VERSION 3.25)
project(LightningGL)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic -O3")
# when debugging add -g -fsanitize=address optimization flag -O3

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(LightningGL src/main.cpp src/PoggerGL.cpp src/Node.cpp
                           src/thunder.cpp src/shaderProgram.cpp)

target_link_libraries(LightningGL OpenGL::GL GLEW::GLEW GLUT::GLUT glm::glm)
