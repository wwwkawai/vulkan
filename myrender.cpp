//
// Created by 87549 on 2023/2/13.
//

#include "myrender.hpp"
namespace myrender {
    void Init(const std::vector<const char *> &extensions,CreateSurfaceFunc func, int w, int h) {
        Context::Init(extensions,func);
        Context::GetInstance().InitSwapchain(w,h);
        Shader::Init(ReadWholeFile("./vert.spv"), ReadWholeFile("./frag.spv"));
    }

    void Quit() {
        Context::GetInstance().DestroySwapchain();
        Shader::Quit();
        Context::Quit();
    }
}