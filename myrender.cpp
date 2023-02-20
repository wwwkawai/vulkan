//
// Created by 87549 on 2023/2/13.
//

#include "myrender.hpp"
namespace myrender {
    void Init(const std::vector<const char *> &extensions,CreateSurfaceFunc func, int w, int h) {
        Context::Init(extensions,func);
        Context::GetInstance().InitSwapchain(w,h);
        Shader::Init(ReadWholeFile("./vert.spv"), ReadWholeFile("./frag.spv"));
        Context::GetInstance().renderProcess->InitRenderPass();
        Context::GetInstance().renderProcess->InitLayout();
        Context::GetInstance().swapchain->CreateFramebuffers(w,h);
        Context::GetInstance().renderProcess->InitpPipeline(w,h);

    }

    void Quit() {
        Context::GetInstance().renderProcess.reset();
        Context::GetInstance().DestroySwapchain();
        Shader::Quit();
        Context::Quit();
    }
}