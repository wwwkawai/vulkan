//
// Created by 87549 on 2023/2/13.
//

#ifndef VULKAN_MYRENDER_HPP
#define VULKAN_MYRENDER_HPP
#include "context.hpp"
#include "shader.hpp"
namespace myrender{
    void Init(const std::vector<const char *> &extensions, CreateSurfaceFunc func, int w, int h);
    void Quit();
    inline Render& GetRender(){
        return *Context::GetInstance().render;
    }
}
#endif //VULKAN_MYRENDER_HPP
