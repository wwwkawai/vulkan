//
// Created by 87549 on 2023/2/21.
//

#ifndef VULKAN_RENDER_HPP
#define VULKAN_RENDER_HPP
#include "vulkan/vulkan.hpp"
namespace myrender{
    class Render final{
    public:
        Render();
        ~Render();
        void Rendering();

    private:
        int MAX_FRAME_SIZE;
        int cur_frame;
        vk::CommandPool cmdPool;
        std::vector<vk::CommandBuffer> cmdBuffer;
        std::vector<vk::Semaphore> imageAvailable;
        std::vector<vk::Semaphore> imageDrawFinish;
        std::vector<vk::Fence> cmdAvailable;
        void InitCmdPool();
        void AllocCmdBuf();
        void CreateFences();
        void CreateSemaphores();

    };
}

#endif //VULKAN_RENDER_HPP
