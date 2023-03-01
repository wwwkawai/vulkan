//
// Created by 87549 on 2023/2/25.
//

#include "buffer.hpp"
#include "context.hpp"
namespace myrender{
    Buffer::Buffer(size_t size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property):size(size) {
        CreateBuffer(size, usage);
        auto info = QueryMemoryInfo(property);
        AllocateMemory(info);
        BindMem2Buf();
        if(property & vk::MemoryPropertyFlagBits::eHostVisible){
            ptr = Context::GetInstance().device.mapMemory(memory, 0, size);
        }else{
            ptr = nullptr;
        }
    }
    Buffer::~Buffer() {
        auto& device = Context::GetInstance().device;
        if(ptr){
            device.unmapMemory(memory);
        }
        device.freeMemory(memory);
        device.destroyBuffer(buffer);
    }
    void Buffer::CreateBuffer(size_t size, vk::BufferUsageFlags usage) {
        vk::BufferCreateInfo createInfo;
        createInfo.setSize(size)
        .setUsage(usage)
        .setSharingMode(vk::SharingMode::eExclusive);
        buffer = Context::GetInstance().device.createBuffer(createInfo);
    }
    void Buffer::AllocateMemory(MemoryInfo info) {
        vk::MemoryAllocateInfo allocateInfo;
        allocateInfo.setAllocationSize(info.size)
        .setMemoryTypeIndex(info.index);
        memory = Context::GetInstance().device.allocateMemory(allocateInfo);

    }
    void Buffer::BindMem2Buf() {
        Context::GetInstance().device.bindBufferMemory(buffer,memory,0);
    }
    Buffer::MemoryInfo Buffer::QueryMemoryInfo(vk::MemoryPropertyFlags property) {
        MemoryInfo memoryInfo;

        auto requirements = Context::GetInstance().device.getBufferMemoryRequirements(buffer);
        memoryInfo.size = requirements.size;
        auto properties = Context::GetInstance().phyDevice.getMemoryProperties();
        for(int i=0;i<properties.memoryTypeCount;i++){
            if((1<<i)&requirements.memoryTypeBits &&
            properties.memoryTypes[i].propertyFlags&property){
                memoryInfo.index = i;
                break;
            }
        }
        return memoryInfo;
    }
}