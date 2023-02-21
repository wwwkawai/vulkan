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
        vk::CommandPool cmdPool;
        vk::CommandBuffer cmdBuf;
        vk::Semaphore imageAvaliable;
        vk::Semaphore imageDrawFinish;
        vk::Fence cmdAvailble;
        void InitCmdPool();
        void AllocCmdBuf();
        void CreateFences();
        void CreateSemaphores();

    };
}

#endif //VULKAN_RENDER_HPP
