cmake_minimum_required(VERSION 3.26)
project(ZoEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

# Define the executable
add_executable(ZoEngine
        src/main.cpp
        src/tools/Logger.cpp
        src/window/Window.cpp
)

# Include Vulkan header directories
target_include_directories(ZoEngine PRIVATE
        ${Vulkan_INCLUDE_DIRS}
        $ENV{GLFW_INCLUDE_DIR}
        $ENV{GLFW_LIBRARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/tools
        ${CMAKE_CURRENT_SOURCE_DIR}/src/window
)

target_link_libraries(ZoEngine PRIVATE
        ${Vulkan_LIBRARIES}
        glfw
)
