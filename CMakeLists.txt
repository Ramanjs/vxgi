cmake_minimum_required(VERSION 3.22)

project(vxgi VERSION 1.0)

set(TARGET ${PROJECT_NAME})
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(deps)

set(SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/shader.cpp"
    "${PROJECT_SOURCE_DIR}/src/shader.h"
    "${PROJECT_SOURCE_DIR}/src/utils.h"
    "${PROJECT_SOURCE_DIR}/src/utils.cpp"
)

add_executable(${TARGET} ${SOURCES})

add_custom_target(format "clang-format" -i ${SOURCES} COMMENT "Formatting source code...")

target_include_directories(${TARGET} PRIVATE
${PROJECT_SOURCE_DIR}/src
${PROJECT_SOURCE_DIR}/deps/imgui
${GLFW_INCLUDE_DIRS}
${OPENGL_INCLUDE_DIR}
${GLM_INCLUDE_DIRS/../include}
)

target_link_libraries(${TARGET} ${OPENGL_LIBRARIES} glfw GLEW::GLEW imgui)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
