//
// Created by 87549 on 2023/2/21.
//

#ifndef VULKAN_RENDER_HPP
#define VULKAN_RENDER_HPP
#include "vulkan/vulkan.hpp"
#include "memory"
#include "buffer.hpp"
#include "texture.hpp"
namespace myrender{
    class Render final{
    public:
        Render();
        ~Render();
        void Rendering();
        void Start();
        void End();
        void Rendering(Texture* texture);

    private:
        int MAX_FRAME_SIZE;
        int cur_frame;
        std::vector<vk::CommandBuffer> cmdBuffer;
        std::vector<vk::Semaphore> imageAvailable;
        std::vector<vk::Semaphore> imageDrawFinish;
        std::vector<vk::Fence> cmdAvailable;
        std::unique_ptr<Buffer> hostVertexBuf;
        std::unique_ptr<Buffer> deviceVertexBuf;
        std::unique_ptr<Buffer> hostIndicesBuf;
        std::unique_ptr<Buffer> deviceIndicesBuf;
        std::vector<std::unique_ptr<Buffer>> hostUniformBuf;
        std::vector<std::unique_ptr<Buffer>> deviceUniformBuf;
        vk::DescriptorPool descPool;
        std::vector<vk::DescriptorSet> descSets;
        vk::Sampler sampler;
        std::unique_ptr<Texture> texture;
        unsigned int imageIndex;
        void CreateVertexBuf();
        void CreateUniformBuf();
        void BufVertexData();
        void BufUniformData();
        void AllocCmdBuf();
        void CreateSets();
        void CreateFences();
        void CreateSemaphores();
        void CreateDescriptorPool();
        void UpdateDescriptorSets();
        void CreateIndicesBuf();
        void BufIndicesData();
        void InitMVP();
        void CreateSampler();
        void CreateTexture();
        void CopyFromBuf(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);

    };
}

#endif //VULKAN_RENDER_HPP
