//
// Created by 87549 on 2023/2/13.
//
#define TINYOBJLOADER_IMPLEMENTATION
#include "SDL.h"
#include "SDL_vulkan.h"
#include <iostream>
#include "myrender.hpp"
#include "tiny_obj_loader.h"
void LoadObj(std::vector<myrender::Vertex>& vertices, std::vector<uint32_t>& indices, std::string objPath){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str()))
        throw std::runtime_error("Load model failed!");
    for(auto& shape: shapes){
        for (const auto& index : shape.mesh.indices) {
            myrender::Vertex vertex(glm::vec3(attrib.vertices[3 * index.vertex_index + 0],attrib.vertices[3 * index.vertex_index + 1],attrib.vertices[3 * index.vertex_index + 2]),
                                    glm::vec3(1.,1.,1.),
                                    glm::vec2(attrib.texcoords[2 * index.texcoord_index + 0],1.0f - attrib.texcoords[2 * index.texcoord_index + 1]));
            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }


}
const int width = 800;
const int height = 600;
int main(int , char**) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("sandbox",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          width, height,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

    if (!window) {
        SDL_Log("Creat window failed");
        exit(2);
    }
    bool windowClose = false;
    SDL_Event event;
    unsigned int count;
    SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
    std::vector<const char *> extentions(count);
    SDL_Vulkan_GetInstanceExtensions(window, &count, extentions.data());

    myrender::Init(extentions,[&](vk::Instance instance){
        VkSurfaceKHR surface;
        if(!SDL_Vulkan_CreateSurface(window, instance, &surface)){
            throw std::runtime_error("create surface failed");
        }
        return surface;
    }, width, height);
    auto& render = myrender::GetRender();

    std::vector<myrender::Vertex> vertices1 = {
            myrender::Vertex(glm::vec3(-0.5,0.0,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,0.0)),
            myrender::Vertex(glm::vec3(0.0,0.0,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,1.0)),
            myrender::Vertex(glm::vec3(0.0,0.5,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,1.0)),
            myrender::Vertex(glm::vec3(-0.5,0.5,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,0.0)),
            myrender::Vertex(glm::vec3(-0.2,-0.3,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,1.0)),
            myrender::Vertex(glm::vec3(0.3,-0.3,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,0.0)),
            myrender::Vertex(glm::vec3(0.3,0.2,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,0.0)),
            myrender::Vertex(glm::vec3(-0.2,0.2,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,1.0))
    };
    std::vector<myrender::Vertex> vertices2 = {
            myrender::Vertex(glm::vec3(-0.2,-0.3,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,1.0)),
            myrender::Vertex(glm::vec3(0.3,-0.3,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,0.0)),
            myrender::Vertex(glm::vec3(0.3,0.2,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,0.0)),
            myrender::Vertex(glm::vec3(-0.2,0.2,-0.3),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,1.0))
    };
    std::string texturePath = "./model/viking_room.png";
    std::vector<myrender::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::string objPath = "./model/viking_room.obj";
    LoadObj(vertices,indices,objPath);
    myrender::Object* obj = new myrender::Object(vertices,indices,texturePath);

    myrender::Object* obj1 = new myrender::Object(vertices1,std::vector<uint32_t>{0,1,2,2,3,0,4,5,6,6,7,4},texturePath);
    //myrender::Object* obj2 = new myrender::Object(vertices2, std::vector<uint32_t>{4,5,6,6,7,4},"./texture.jpg");
    render.AddObj(obj);
    render.AddObj(obj1);
    //render.AddObj(obj2);
    while (!windowClose) {
        render.Start();
        render.Rendering();
        render.End();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                windowClose = true;
            }
        }
    }
    myrender::Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}