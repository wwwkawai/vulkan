//
// Created by 87549 on 2023/2/25.
//

#ifndef VULKAN_COMMAND_HPP
#define VULKAN_COMMAND_HPP
#include "vulkan/vulkan.hpp"
#include <functional>
namespace myrender {
    class CommandManager final {
    public:
        CommandManager();

        ~CommandManager();

        vk::CommandBuffer CreateOneCommandBuffer();

        std::vector<vk::CommandBuffer> CreateCommandBuffers(std::uint32_t count);

        void ResetCmds();

        void FreeCmd(vk::CommandBuffer& buffer);
        using WriteCmdBuffer = std::function<void(vk::CommandBuffer&)>;
        void ExecuteCmd(vk::Queue queue, WriteCmdBuffer func);

    private:
        vk::CommandPool pool;

        vk::CommandPool CreateCommandPool();


    };


}
#endif //VULKAN_COMMAND_HPP
