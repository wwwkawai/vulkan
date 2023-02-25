//
// Created by 87549 on 2023/2/21.
//

#include "render.hpp"
#include "context.hpp"
#include <limits>
namespace myrender {
    Render::Render() {
        MAX_FRAME_SIZE = 2;
        cur_frame = 0;
        InitCmdPool();
        AllocCmdBuf();
        CreateSemaphores();
        CreateFences();
    }

    Render::~Render() {
        auto &device = Context::GetInstance().device;
        if (!device)
            std::cout << "Access device failed!" << std::endl;
        for(auto& cmdBuf:cmdBuffer){
            device.freeCommandBuffers(cmdPool, cmdBuf);
        }
        device.destroyCommandPool(cmdPool);
        for (auto &imageAvai: imageAvailable) {
            device.destroySemaphore(imageAvai);
        }
        for (auto &imageDraw: imageDrawFinish) {
            device.destroySemaphore(imageDraw);
        }

        for (auto &cmdAvai: cmdAvailable) {
            device.destroyFence(cmdAvai);
        }


    }

    void Render::InitCmdPool() {
        vk::CommandPoolCreateInfo createInfo;
        createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        cmdPool = Context::GetInstance().device.createCommandPool(createInfo);
    }

    void Render::AllocCmdBuf() {
        cmdBuffer.resize(MAX_FRAME_SIZE);
        for(int i=0;i<MAX_FRAME_SIZE;i++) {
            vk::CommandBufferAllocateInfo allocateInfo;
            allocateInfo.setCommandPool(cmdPool)
                    .setCommandBufferCount(1)
                    .setLevel(vk::CommandBufferLevel::ePrimary);
            cmdBuffer[i] = Context::GetInstance().device.allocateCommandBuffers(allocateInfo)[0];
        }

    }

    void Render::CreateFences() {
        cmdAvailable.resize(MAX_FRAME_SIZE);
        for (int i = 0; i < MAX_FRAME_SIZE; i++) {
            vk::FenceCreateInfo createInfo;
            createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            cmdAvailable[i] = Context::GetInstance().device.createFence(createInfo);
        }
    }

    void Render::CreateSemaphores() {
        auto &device = Context::GetInstance().device;
        imageAvailable.resize(MAX_FRAME_SIZE);
        imageDrawFinish.resize(MAX_FRAME_SIZE);
        for (int i = 0; i < MAX_FRAME_SIZE; i++) {
            vk::SemaphoreCreateInfo createInfo;
            imageAvailable[i] = device.createSemaphore(createInfo);
            imageDrawFinish[i] = device.createSemaphore(createInfo);
        }
    }

    void Render::Rendering() {
        auto &device = Context::GetInstance().device;
        auto &swapchain = Context::GetInstance().swapchain;
        auto &pipeline = Context::GetInstance().renderProcess->pipeline;
        auto &renderProcess = Context::GetInstance().renderProcess;
        auto &graphicsQueue = Context::GetInstance().graphicsQueue;
        auto &presentQueue = Context::GetInstance().presentQueue;
        if (device.waitForFences(cmdAvailable[cur_frame], true, std::numeric_limits<uint64_t>::max())
            != vk::Result::eSuccess) {
            std::cout << "Wait for fences failed!" << std::endl;
        }
        device.resetFences(cmdAvailable[cur_frame]);

        auto result = device.acquireNextImageKHR(swapchain->swapchain, std::numeric_limits<uint64_t>::max(),
                                                 imageAvailable[cur_frame]);

        if (result.result != vk::Result::eSuccess) {
            std::cout << "Acquire image failed!" << std::endl;
        }
        auto imageIndex = result.value;
        auto& cmdBuf = cmdBuffer[cur_frame];
        cmdBuf.reset();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo);
        {
            cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
            vk::RenderPassBeginInfo renderPassBeginInfo;
            vk::Rect2D area;
            vk::ClearValue clearValue;
            area.setOffset({0, 0})
                    .setExtent(swapchain->info.imageExtent);
            clearValue.color = vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 0.1f});
            renderPassBeginInfo.setRenderPass(renderProcess->renderPass)
                    .setRenderArea(area)
                    .setFramebuffer(swapchain->framebuffers[imageIndex])
                    .setClearValues(clearValue);
            cmdBuf.beginRenderPass(renderPassBeginInfo, {});
            {
                cmdBuf.draw(3, 1, 0, 0);
            }
            cmdBuf.endRenderPass();

        }
        cmdBuf.end();
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuf)
                .setWaitSemaphores(imageAvailable[cur_frame])
                .setSignalSemaphores(imageDrawFinish[cur_frame]);
        graphicsQueue.submit(submitInfo, cmdAvailable[cur_frame]);
        vk::PresentInfoKHR presentInfo;
        presentInfo.setImageIndices(imageIndex)
                .setSwapchains(swapchain->swapchain)
                .setWaitSemaphores(imageDrawFinish[cur_frame]);
        if (presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
            std::cout << "Presents image failed!" << std::endl;
        }

        cur_frame = (cur_frame + 1) % MAX_FRAME_SIZE;

    }
}