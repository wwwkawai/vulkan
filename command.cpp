//
// Created by 87549 on 2023/2/25.
//

#include "command.hpp"
#include "context.hpp"
namespace myrender{
    CommandManager::CommandManager() {
        pool = CreateCommandPool();
    }
    CommandManager::~CommandManager() {
        auto& device = Context::GetInstance().device;
        device.destroyCommandPool(pool);
    }
    std::vector<vk::CommandBuffer> CommandManager::CreateCommandBuffers(std::uint32_t count) {
        vk::CommandBufferAllocateInfo allocateInfo;
        allocateInfo.setCommandPool(pool)
                .setCommandBufferCount(count)
                .setLevel(vk::CommandBufferLevel::ePrimary);
        return Context::GetInstance().device.allocateCommandBuffers(allocateInfo);
    }
    void CommandManager::ResetCmds() {
        Context::GetInstance().device.resetCommandPool(pool);
    }
    vk::CommandBuffer CommandManager::CreateOneCommandBuffer() {
        return CreateCommandBuffers(1)[0];
    }
    vk::CommandPool CommandManager::CreateCommandPool() {
        vk::CommandPoolCreateInfo createInfo;
        createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(Context::GetInstance().queueFamilyIndices.graphicsQueue.value());
        return Context::GetInstance().device.createCommandPool(createInfo);
    }
    void CommandManager::FreeCmd(vk::CommandBuffer& buffer) {
        Context::GetInstance().device.freeCommandBuffers(pool,buffer);
    }

}