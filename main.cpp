//
// Created by 87549 on 2023/2/13.
//
#include "SDL.h"
#include "SDL_vulkan.h"
#include <iostream>
#include "myrender.hpp"
int main(int , char**) {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("sandbox",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          1024, 720,
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
    }, 1024, 720);
    auto& render = myrender::GetRender();
    std::vector<myrender::Vertex> vertices1 = {
            myrender::Vertex(glm::vec3(-0.5,0.0,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,0.0)),
            myrender::Vertex(glm::vec3(0.0,0.0,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,1.0)),
            myrender::Vertex(glm::vec3(0.0,0.5,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,1.0)),
            myrender::Vertex(glm::vec3(-0.5,0.5,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,0.0))
    };
    std::vector<myrender::Vertex> vertices2 = {
            myrender::Vertex(glm::vec3(0,-0.5,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,1.0)),
            myrender::Vertex(glm::vec3(0.5,-0.5,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(1.0,0.0)),
            myrender::Vertex(glm::vec3(0.5,0.0,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,0.0)),
            myrender::Vertex(glm::vec3(0.0,0.0,0.0),glm::vec3(1.0,1.0,1.0),glm::vec2(0.0,1.0))
    };
    myrender::Object* obj1 = new myrender::Object(vertices1,std::vector<uint16_t>{0,1,2,2,3,0},"./role.png");
    myrender::Object* obj2 = new myrender::Object(vertices2, std::vector<uint16_t>{0,1,2,2,3,0},"./texture.jpg");
    render.AddObj(obj1);
    render.AddObj(obj2);
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