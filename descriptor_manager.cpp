//
// Created by 87549 on 2023/3/9.
//

#include "descriptor_manager.hpp"
#include "context.hpp"
namespace myrender{
    std::unique_ptr<DescriptorManager> DescriptorManager::instance = nullptr;
    void DescriptorManager::CreateDescriptorPool(vk::DescriptorType type) {
        vk::DescriptorPoolCreateInfo createInfo;
        vk::DescriptorPoolSize poolSize;
        poolSize.setDescriptorCount(MAX_FRAME_SIZE)
                .setType(type);
        createInfo.setPoolSizes(poolSize)
                .setMaxSets(MAX_FRAME_SIZE);
        descPools.push_back({Context::GetInstance().device.createDescriptorPool(createInfo),MAX_FRAME_SIZE,MAX_FRAME_SIZE,vk::DescriptorType::eUniformBuffer});
    }
    DescriptorManager& DescriptorManager::Instance() {
        return *instance;
    }
    DescriptorManager::~DescriptorManager() {
        auto& device = Context::GetInstance().device;
        for(auto& pool:descPools)
            device.destroyDescriptorPool(pool.pool);
    }
    DescriptorManager::DescriptorManager(int MAX_FRAME_SIZE):MAX_FRAME_SIZE(MAX_FRAME_SIZE) {
        CreateDescriptorPool(vk::DescriptorType::eUniformBuffer);
    }
    DescriptorManager::setInfo DescriptorManager::AllocSet(vk::DescriptorType type, vk::DescriptorSetLayout& layout) {
        for(auto& pool:descPools){
            if(type == pool.type && pool.remain_sets>0){
                std::vector<vk::DescriptorSetLayout> layouts{layout};
                vk::DescriptorSetAllocateInfo allocateInfo;
                allocateInfo.setSetLayouts(layouts)
                        .setDescriptorPool(pool.pool)
                        .setDescriptorSetCount(1);
                auto set = Context::GetInstance().device.allocateDescriptorSets(allocateInfo);
                pool.remain_sets--;
                return {pool.pool,set[0]};
            }
        }
        CreateDescriptorPool(type);
        auto& pool = descPools.back();
        std::vector<vk::DescriptorSetLayout> layouts{layout};
        vk::DescriptorSetAllocateInfo allocateInfo;
        allocateInfo.setSetLayouts(layouts)
                .setDescriptorPool(pool.pool)
                .setDescriptorSetCount(1);
        auto set = Context::GetInstance().device.allocateDescriptorSets(allocateInfo);
        pool.remain_sets--;
        return {pool.pool,set[0]};

    }
    void DescriptorManager::Init(int MAX_FRAME_SIZE) {
        instance.reset(new DescriptorManager(MAX_FRAME_SIZE));
    }
    void DescriptorManager::Quit() {
        instance.reset();
    }

}