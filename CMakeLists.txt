cmake_minimum_required(VERSION 3.26)
project(ZoEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

# Define the executable
FILE(GLOB_RECURSE MyCppSources src/*.cpp)
add_executable(ZoEngine
        ${MyCppSources}
)

# Include Vulkan header directories
target_include_directories(ZoEngine PRIVATE
        ${Vulkan_INCLUDE_DIRS}
        $ENV{GLFW_INCLUDE_DIR}
        $ENV{GLFW_LIBRARY_DIR}
        $ENV{VMA_INCLUDE_PATH}
        $ENV{STB_INCLUDE_PATH}
        $ENV{CMAKE_INCLUDE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/model
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tools
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan
)

target_link_libraries(ZoEngine PRIVATE
        ${Vulkan_LIBRARIES}
        vk-bootstrap
        glfw
)

find_program(GLSLC_EXECUTABLE NAMES glslc)
message("glslc: ${GLSLC_EXECUTABLE}")

# compile shaders
set(SHADER_DIR "src/shader")
file(GLOB GLSL_SOURCE_FILES
        ${SHADER_DIR}/*.frag
        ${SHADER_DIR}/*.vert
)
foreach (GLSL ${GLSL_SOURCE_FILES})
    message("glslc source file: ${GLSL}")
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_DIR}/${FILE_NAME}.spv")
    set(DIRECTORY_COMMAND "${CMAKE_COMMAND} -E make_directory ${SHADER_DIR}")
    set(COMPILE_COMMAND "${GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}")
    message("direcory command: ${DIRECTORY_COMMAND}")
    message("compile command: ${COMPILE_COMMAND}")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_DIR}"
            COMMAND ${GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach ()

add_custom_target(
        Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)
add_dependencies(ZoEngine Shaders)

add_custom_command(TARGET Shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_PROPERTY:ZoEngine,SOURCE_DIR>/${SHADER_DIR}"
        "$<TARGET_PROPERTY:ZoEngine,BINARY_DIR>/$<CONFIGURATION>/${SHADER_DIR}"
)