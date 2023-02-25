//
// Created by 87549 on 2023/2/21.
//

#include "render.hpp"
#include "context.hpp"
#include <limits>
#include "vertex.hpp"
namespace myrender {

    std::array<Vec2f,3> vertices = {
            Vec2f(0.,-0.7),
            Vec2f(0.7,0.7),
            Vec2f(-0.7,0.7)
    };

    Render::Render() {
        MAX_FRAME_SIZE = 2;
        cur_frame = 0;
        AllocCmdBuf();
        CreateSemaphores();
        CreateFences();
        CreateVertexBuf();
        BufVertexData();
    }

    Render::~Render() {
        hostVertexBuf.reset();
        deviceVertexBuf.reset();
        auto &device = Context::GetInstance().device;
        /*
        for(auto &cmdBuf:cmdBuffer) {
            Context::GetInstance().commandManager->FreeCmd(cmdBuf);
        }
         */
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

    void Render::AllocCmdBuf() {
        cmdBuffer = Context::GetInstance().commandManager->CreateCommandBuffers(MAX_FRAME_SIZE);
        /*
        cmdBuffer.resize(MAX_FRAME_SIZE);
        for(int i=0;i<MAX_FRAME_SIZE;i++) {
            cmdBuffer[i] = Context::GetInstance().commandManager->CreateOneCommandBuffer();
        }
         */
    }

    void Render::CreateFences() {
        cmdAvailable.resize(MAX_FRAME_SIZE);
        for (int i = 0; i < MAX_FRAME_SIZE; i++) {
            vk::FenceCreateInfo createInfo;
            createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            cmdAvailable[i] = Context::GetInstance().device.createFence(createInfo);
        }
    }

    void Render::CreateVertexBuf() {
        hostVertexBuf.reset(new Buffer(sizeof(vertices),vk::BufferUsageFlagBits::eTransferSrc,vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent));
        deviceVertexBuf.reset(new Buffer(sizeof(vertices),vk::BufferUsageFlagBits::eTransferDst|vk::BufferUsageFlagBits::eVertexBuffer,vk::MemoryPropertyFlagBits::eDeviceLocal));
    }
    void Render::BufVertexData() {
        void *ptr = Context::GetInstance().device.mapMemory(hostVertexBuf->memory, 0, hostVertexBuf->size);
        memcpy(ptr,vertices.data(),sizeof(vertices));
        Context::GetInstance().device.unmapMemory(hostVertexBuf->memory);
        auto cmdBuf = Context::GetInstance().commandManager->CreateOneCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo); {
            vk::BufferCopy region;
            region.setSize(hostVertexBuf->size)
            .setSrcOffset(0)
            .setDstOffset(0);
            cmdBuf.copyBuffer(hostVertexBuf->buffer,deviceVertexBuf->buffer,region);
        } cmdBuf.end();
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuf);
        Context::GetInstance().graphicsQueue.submit(submitInfo);
        Context::GetInstance().device.waitIdle();
        Context::GetInstance().commandManager->FreeCmd(cmdBuf);



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

            vk::DeviceSize offset = 0;
            cmdBuf.bindVertexBuffers(0, deviceVertexBuf->buffer, offset);
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