
cmake_minimum_required(VERSION 3.23)
project(vulkan)
include_directories("E:/master/vulkan/glm")
set(CMAKE_CXX_STANDARD 17)
set(SDL_DIR "D:/Program Files/SDL2-2.26.3/x86_64-w64-mingw32/lib/cmake/SDL2/sdl2-config.cmake")
include("${SDL_DIR}")
set(VULKAN_DIR "D:/Program Files/vulkan")
set(VULKAN_INCLUDE_DIR "${VULKAN_DIR}/Include")
set(VULKAN_LIB_LOCATION "${VULKAN_DIR}/Lib/vulkan-1.lib")

find_program(GLSLC_PROGRAM glslc REQUIRED)
message(STATUS "Compiling shaders ...")
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shader.vert -o ${CMAKE_SOURCE_DIR}/vert.spv)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shader.frag -o ${CMAKE_SOURCE_DIR}/frag.spv)
message(STATUS "Compiling shader OK")


add_library(Vulkan STATIC IMPORTED GLOBAL)
set_target_properties(
        Vulkan
        PROPERTIES
        IMPORTED_LOCATION ${VULKAN_LIB_LOCATION}
        INTERFACE_INCLUDE_DIRECTORIES ${VULKAN_INCLUDE_DIR}
)

add_executable(vulkan main.cpp context.cpp context.hpp myrender.cpp myrender.hpp tool.hpp swapchain.cpp swapchain.hpp shader.cpp shader.hpp tool.cpp renderprocess.cpp renderprocess.hpp render.cpp render.hpp buffer.cpp buffer.hpp command.cpp command.hpp uniform.hpp object.hpp object.cpp texture.cpp texture.hpp stb_image.h descriptor_manager.cpp descriptor_manager.hpp scene.cpp scene.hpp) #object.cpp object.hpp scene.cpp scene.hpp
target_link_libraries(vulkan Vulkan SDL2::SDL2main SDL2::SDL2)