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
    while (!windowClose) {
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