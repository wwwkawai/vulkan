//
// Created by 87549 on 2023/2/21.
//

#ifndef VULKAN_RENDER_HPP
#define VULKAN_RENDER_HPP
#include "vulkan/vulkan.hpp"
#include "memory"
#include "buffer.hpp"
namespace myrender{
    class Render final{
    public:
        Render();
        ~Render();
        void Rendering();

    private:
        int MAX_FRAME_SIZE;
        int cur_frame;
        std::vector<vk::CommandBuffer> cmdBuffer;
        std::vector<vk::Semaphore> imageAvailable;
        std::vector<vk::Semaphore> imageDrawFinish;
        std::vector<vk::Fence> cmdAvailable;
        std::unique_ptr<Buffer> hostVertexBuf;
        std::unique_ptr<Buffer> deviceVertexBuf;
        void CreateVertexBuf();
        void BufVertexData();
        void AllocCmdBuf();
        void CreateFences();
        void CreateSemaphores();

    };
}

#endif //VULKAN_RENDER_HPP
