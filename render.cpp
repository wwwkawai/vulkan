//
// Created by 87549 on 2023/2/21.
//

#include "render.hpp"
#include "context.hpp"
#include <limits>
namespace myrender{
    Render::Render() {
        InitCmdPool();
        AllocCmdBuf();
        CreateSemaphores();
        CreateFences();
    }
    Render::~Render() {
        auto& device = Context::GetInstance().device;
        if(!device)
            std::cout<<"Access device failed!"<<std::endl;
        device.freeCommandBuffers(cmdPool,cmdBuf);
        device.destroyCommandPool(cmdPool);
        device.destroySemaphore(imageAvaliable);
        device.destroySemaphore(imageDrawFinish);
        device.destroyFence(cmdAvailble);


    }
    void Render::InitCmdPool() {
        vk::CommandPoolCreateInfo createInfo;
        createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        cmdPool = Context::GetInstance().device.createCommandPool(createInfo);
    }
    void Render::AllocCmdBuf() {
        vk::CommandBufferAllocateInfo allocateInfo;
        allocateInfo.setCommandPool(cmdPool)
        .setCommandBufferCount(1)
        .setLevel(vk::CommandBufferLevel::ePrimary);
        cmdBuf = Context::GetInstance().device.allocateCommandBuffers(allocateInfo)[0];

    }
    void Render::CreateFences() {
        vk::FenceCreateInfo createInfo;
        cmdAvailble = Context::GetInstance().device.createFence(createInfo);
    }
    void Render::CreateSemaphores() {
        auto& device = Context::GetInstance().device;
        vk::SemaphoreCreateInfo createInfo;
        imageAvaliable = device.createSemaphore(createInfo);
        imageDrawFinish = device.createSemaphore(createInfo);
    }
    void Render::Rendering() {
        auto& device = Context::GetInstance().device;
        auto& swapchain = Context::GetInstance().swapchain;
        auto& pipeline = Context::GetInstance().renderProcess->pipeline;
        auto& renderProcess = Context::GetInstance().renderProcess;
        auto& graphicsQueue = Context::GetInstance().graphicsQueue;
        auto& presentQueue = Context::GetInstance().presentQueue;
        auto result = device.acquireNextImageKHR(swapchain->swapchain,std::numeric_limits<uint64_t>::max(),imageAvaliable);

        if(result.result != vk::Result::eSuccess){
            std::cout<<"Acquire image failed!"<<std::endl;
        }
        auto imageIndex = result.value;
        cmdBuf.reset();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo);{
            cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics,pipeline);
            vk::RenderPassBeginInfo renderPassBeginInfo;
            vk::Rect2D area;
            vk::ClearValue clearValue;
            area.setOffset({0,0})
            .setExtent(swapchain->info.imageExtent);
            clearValue.color = vk::ClearColorValue(std::array<float,4>{0.1f,0.1f,0.1f,0.1f});
            renderPassBeginInfo.setRenderPass(renderProcess->renderPass)
            .setRenderArea(area)
            .setFramebuffer(swapchain->framebuffers[imageIndex])
            .setClearValues(clearValue);
            cmdBuf.beginRenderPass(renderPassBeginInfo,{});{
                cmdBuf.draw(3,1,0,0);
            }
            cmdBuf.endRenderPass();

        }
        cmdBuf.end();
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuf)
        .setWaitSemaphores(imageAvaliable)
        .setSignalSemaphores(imageDrawFinish);
        graphicsQueue.submit(submitInfo,cmdAvailble);
        vk::PresentInfoKHR presentInfo;
        presentInfo.setImageIndices(imageIndex)
        .setSwapchains(swapchain->swapchain)
        .setWaitSemaphores(imageDrawFinish);
        if(presentQueue.presentKHR(presentInfo)!=vk::Result::eSuccess){
            std::cout<<"Presents image failed!"<<std::endl;
        }

        if(device.waitForFences(cmdAvailble,true,std::numeric_limits<uint64_t>::max())
           != vk::Result::eSuccess){
            std::cout<<"Wait for fences failed!"<<std::endl;
        }
        device.resetFences(cmdAvailble);

    }
}