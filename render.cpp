//
// Created by 87549 on 2023/2/21.
//
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "render.hpp"
#include "context.hpp"
#include <limits>
#include "object.hpp"
#include "uniform.hpp"
#include <glm/gtc/matrix_transform.hpp>
namespace myrender {
    Uniform ubo;

    Render::Render() {

        MAX_FRAME_SIZE = 2;
        DescriptorManager::Init(MAX_FRAME_SIZE);
        scene.reset(new Scene());
        cur_frame = 0;
        AllocCmdBuf();
        CreateSemaphores();
        CreateFences();
        /*
        CreateVertexBuf();
        BufVertexData();
        CreateIndicesBuf();
        BufIndicesData();
        */
        InitMVP();
        CreateUniformBuf();
        BufUniformData();
        CreateTexture();
        CreateSampler();
        CreateDescriptorPool();
        CreateSets();
        UpdateDescriptorSets();
    }

    Render::~Render() {
        scene.reset();

        auto &device = Context::GetInstance().device;

        device.destroyDescriptorPool(descPool);
        hostUniformBuf.clear();
        deviceUniformBuf.clear();
        for(auto &cmdBuf:cmdBuffer) {
            Context::GetInstance().commandManager->FreeCmd(cmdBuf);
        }
        device.destroySampler(sampler);
        texture.reset();
        for (auto &imageAvai: imageAvailable) {
            device.destroySemaphore(imageAvai);
        }
        for (auto &imageDraw: imageDrawFinish) {
            device.destroySemaphore(imageDraw);
        }

        for (auto &cmdAvai: cmdAvailable) {
            device.destroyFence(cmdAvai);
        }
        DescriptorManager::Quit();

    }

    void Render::AddObj(Object *obj) {
        scene->AddObj(obj);
    }
    void Render::DeleteObj(Object *obj) {
        scene->DeleteObj(obj);
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
    void Render::InitMVP() {
        ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        int width = Context::GetInstance().swapchain->info.imageExtent.width;
        int height = Context::GetInstance().swapchain->info.imageExtent.height;
        ubo.proj = glm::perspective(glm::radians(45.0f),(float)width/(float)height,0.1f,10.0f);
        ubo.proj[1][1]*=-1.0;
    }
    void Render::CreateFences() {
        cmdAvailable.resize(MAX_FRAME_SIZE);
        for (int i = 0; i < MAX_FRAME_SIZE; i++) {
            vk::FenceCreateInfo createInfo;
            createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
            cmdAvailable[i] = Context::GetInstance().device.createFence(createInfo);
        }
    }

    void Render::CreateUniformBuf() {
        hostUniformBuf.resize(MAX_FRAME_SIZE);
        deviceUniformBuf.resize(MAX_FRAME_SIZE);
        for(int i=0;i<MAX_FRAME_SIZE;i++){
            hostUniformBuf[i].reset(new Buffer(sizeof(Uniform),vk::BufferUsageFlagBits::eTransferSrc,vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent));
            deviceUniformBuf[i].reset(new Buffer(sizeof(Uniform),vk::BufferUsageFlagBits::eUniformBuffer|vk::BufferUsageFlagBits::eTransferDst,vk::MemoryPropertyFlagBits::eDeviceLocal));
        }
    }



    void Render::BufUniformData() {
        for (int i = 0; i < MAX_FRAME_SIZE; i++) {
            auto &buffer = hostUniformBuf[i];
            memcpy(buffer->ptr, &ubo, sizeof(ubo));
            CopyFromBuf(buffer->buffer, deviceUniformBuf[i]->buffer, buffer->size, 0, 0);
        }
    }
    void Render::CopyFromBuf(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset) {
        auto cmdBuf = Context::GetInstance().commandManager->CreateOneCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo); {
            vk::BufferCopy region;
            region.setSize(size)
                    .setSrcOffset(srcOffset)
                    .setDstOffset(dstOffset);
            cmdBuf.copyBuffer(src,dst,region);
        } cmdBuf.end();
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuf);
        Context::GetInstance().graphicsQueue.submit(submitInfo);
        Context::GetInstance().device.waitIdle();
        Context::GetInstance().commandManager->FreeCmd(cmdBuf);
    }
    void Render::CreateDescriptorPool() {
        vk::DescriptorPoolCreateInfo createInfo;
        std::vector<vk::DescriptorPoolSize> poolSize(1);
        poolSize[0].setDescriptorCount(MAX_FRAME_SIZE)
        .setType(vk::DescriptorType::eUniformBuffer);
        createInfo.setPoolSizes(poolSize)
        .setMaxSets(MAX_FRAME_SIZE);
        descPool = Context::GetInstance().device.createDescriptorPool(createInfo);
    }
    void Render::CreateSets() {
        /*
        std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAME_SIZE, Context::GetInstance().renderProcess->setLayout[0]);
        vk::DescriptorSetAllocateInfo allocateInfo;
        allocateInfo.setSetLayouts(layouts)
        .setDescriptorPool(descPool)
        .setDescriptorSetCount(MAX_FRAME_SIZE);
        descSets = Context::GetInstance().device.allocateDescriptorSets(allocateInfo);
        */
        for(int i=0;i<MAX_FRAME_SIZE;i++) {
            descSets.push_back(DescriptorManager::Instance().AllocSet(vk::DescriptorType::eUniformBuffer, Context::GetInstance().renderProcess->setLayout[0]).set);
        }
    }
    void Render::UpdateDescriptorSets() {
        for(int i=0;i<descSets.size();i++){
            auto& set = descSets[i];
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.setOffset(0)
            .setBuffer(deviceUniformBuf[i]->buffer)
            .setRange(deviceUniformBuf[i]->size);

            vk::WriteDescriptorSet writer;
            writer.setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setDstBinding(0)
            .setBufferInfo(bufferInfo)
            .setDstSet(set)
            .setDstArrayElement(0);
            /*
            vk::DescriptorImageInfo imageInfo;
            imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(texture->imageView)
            .setSampler(sampler);

            writer[1].setImageInfo(imageInfo)
            .setDstBinding(1)
            .setDstArrayElement(0)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setDstSet(set);
            */

            Context::GetInstance().device.updateDescriptorSets(writer,{});
        }


    }

    void Render::CreateSampler() {
        vk::SamplerCreateInfo createInfo;
        createInfo.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setAddressModeU(vk::SamplerAddressMode::eRepeat)
        .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        .setAnisotropyEnable(false)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(false)
        .setCompareEnable(false);
        sampler = Context::GetInstance().device.createSampler(createInfo);
    }
    void Render::CreateTexture() {
        //texture.reset(new Texture("./role.png"));
        texture.reset(new Texture("./texture.jpg"));
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
    void Render::Start() {

        auto &device = Context::GetInstance().device;
        auto &swapchain = Context::GetInstance().swapchain;
        auto &pipeline = Context::GetInstance().renderProcess->pipeline;
        auto &renderProcess = Context::GetInstance().renderProcess;
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
        imageIndex = result.value;
        auto &cmdBuf = cmdBuffer[cur_frame];
        cmdBuf.reset();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo);
        cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        vk::RenderPassBeginInfo renderPassBeginInfo;
        vk::Rect2D area;
        std::array<vk::ClearValue,2> clearValues;
        area.setOffset({0, 0})
                .setExtent(swapchain->info.imageExtent);
        clearValues[0].color = vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 0.1f});
        clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.,0);
        renderPassBeginInfo.setRenderPass(renderProcess->renderPass)
                .setRenderArea(area)
                .setFramebuffer(swapchain->framebuffers[imageIndex])
                .setClearValues(clearValues);
        cmdBuf.beginRenderPass(renderPassBeginInfo, {});
    }
    void Render::Rendering(myrender::Object *obj) {
        auto &renderProcess = Context::GetInstance().renderProcess;
        auto &cmdBuf = cmdBuffer[cur_frame];
        cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Context::GetInstance().renderProcess->layout, 0,
                                  {descSets[cur_frame],obj->texture->descSet.set}, {});

        vk::DeviceSize offset = 0;
        cmdBuf.bindVertexBuffers(0, obj->deviceVertexBuf->buffer, offset);
        cmdBuf.bindIndexBuffer(obj->deviceIndicesBuf->buffer, offset, vk::IndexType::eUint32);
        /*
        cmdBuf.beginRenderPass(renderPassBeginInfo, {});
        {
            auto model = glm::mat4(1.0f);
            cmdBuf.pushConstants(renderProcess->layout,vk::ShaderStageFlagBits::eVertex,0, sizeof(glm::mat4),&model);
            cmdBuf.drawIndexed(indices.size(),1,0,0,0);
            //cmdBuf.draw(3, 1, 0, 0);
        }
        */
        auto model = glm::mat4(1.0f);
        cmdBuf.pushConstants(renderProcess->layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &model);
        cmdBuf.drawIndexed(obj->indices.size(), 1, 0, 0, 0);
    }
    void Render::Rendering() {
        for(auto obj:scene->scene){
            Rendering(obj);
        }
    }
    void Render::End() {
        auto& cmdBuf = cmdBuffer[cur_frame];
        auto &swapchain = Context::GetInstance().swapchain;
        auto &graphicsQueue = Context::GetInstance().graphicsQueue;
        auto &presentQueue = Context::GetInstance().presentQueue;
        cmdBuf.endRenderPass();
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