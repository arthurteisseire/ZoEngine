cmake_minimum_required(VERSION 3.26)
project(ZoEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

# Define the executable
file(GLOB_RECURSE MyCppSources src/*.cpp)

# Tinygltf
file(GLOB TinyGltfSources
        ${CMAKE_CURRENT_SOURCE_DIR}/library/tiny_gltf/tiny_gltf.cc
)

find_path(IMGUI_BASE_DIR NAMES imgui/imgui.h HINTS ${CMAKE_INCLUDE_PATH})
if (!IMGUI_BASE_DIR)
    message("Can't find imgui include dir in CMAKE_INCLUDE_PATH: ${CMAKE_INCLUDE_PATH}")
endif ()
set(IMGUI_DIR "${IMGUI_BASE_DIR}/imgui")

file(GLOB IMGUI_SOURCES
        ${IMGUI_DIR}/*.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)
add_executable(ZoEngine
        ${MyCppSources}
        ${IMGUI_SOURCES}
        ${TinyGltfSources}
)


# Include Vulkan header directories
target_include_directories(ZoEngine PRIVATE
        ${Vulkan_INCLUDE_DIRS}
        $ENV{GLFW_INCLUDE_DIR}
        $ENV{GLFW_LIBRARY_DIR}
        $ENV{VMA_INCLUDE_PATH}
        $ENV{STB_INCLUDE_PATH}
        ${IMGUI_DIR}
        ${CMAKE_INCLUDE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/library/tiny_gltf
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tools
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

target_link_libraries(ZoEngine PRIVATE
        ${Vulkan_LIBRARIES}
        vk-bootstrap
        glfw
)

find_program(GLSLC_EXECUTABLE NAMES glslc)
message("glslc: ${GLSLC_EXECUTABLE}")

# compile shaders
# Define shader directories
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/src/shader")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")


# Find shaders
file(GLOB SHADER_FILES
        ${SHADER_DIR}/*.frag
        ${SHADER_DIR}/*.vert
)
#set(SHADER_FILES
#        ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert
#        ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag
#)

set(COMPILED_SHADERS_DIR ${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders)
file(MAKE_DIRECTORY ${COMPILED_SHADERS_DIR})

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    add_custom_command(
            OUTPUT ${COMPILED_SHADERS_DIR}/${SHADER_NAME}.spv
            COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${COMPILED_SHADERS_DIR}/${SHADER_NAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Compiling ${SHADER_NAME} to SPIR-V"
    )
    list(APPEND COMPILED_SHADER_OUTPUTS ${COMPILED_SHADERS_DIR}/${SHADER_NAME}.spv)
endforeach()

# Custom target to compile all shaders
add_custom_target(compile_shaders ALL
        DEPENDS ${COMPILED_SHADER_OUTPUTS}
)

add_dependencies(ZoEngine compile_shaders)
