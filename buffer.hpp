//
// Created by 87549 on 2023/2/25.
//

#ifndef VULKAN_BUFFER_HPP
#define VULKAN_BUFFER_HPP
#include "vulkan/vulkan.hpp"
namespace myrender {
    class Buffer final {
    public:
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        size_t size;
        void* ptr;
        Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property);

        ~Buffer();

    private:
        struct MemoryInfo final{
            size_t size;
            uint32_t index;
        };

        void CreateBuffer(size_t size, vk::BufferUsageFlags usage);

        void AllocateMemory(MemoryInfo info);

        void BindMem2Buf();

        MemoryInfo QueryMemoryInfo(vk::MemoryPropertyFlags property);
    };
    std::uint32_t QueryBufferMemTypeIndex(std::uint32_t type, vk::MemoryPropertyFlags flag);

}
#endif //VULKAN_BUFFER_HPP
