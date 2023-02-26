//
// Created by 87549 on 2023/2/13.
//

#include "myrender.hpp"
namespace myrender {
    void Init(const std::vector<const char *> &extensions,CreateSurfaceFunc func, int w, int h) {
        Context::Init(extensions,func);
        Context::GetInstance().InitSwapchain(w,h);
        Shader::Init(ReadWholeFile("./vert.spv"), ReadWholeFile("./frag.spv"));
        Context::GetInstance().renderProcess.reset(new RenderProcess);
        Context::GetInstance().swapchain->CreateFramebuffers(w,h);
        Context::GetInstance().renderProcess->InitpPipeline(w,h);
        Context::GetInstance().InitCommandManager();
        Context::GetInstance().InitRender();
    }

    void Quit() {
        Context::GetInstance().device.waitIdle();
        Context::GetInstance().render.reset();
        Context::GetInstance().commandManager.reset();
        Context::GetInstance().renderProcess.reset();
        Context::GetInstance().DestroySwapchain();
        Shader::Quit();
        Context::Quit();
    }
}