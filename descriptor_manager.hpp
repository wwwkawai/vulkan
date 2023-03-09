//
// Created by 87549 on 2023/3/9.
//

#ifndef VULKAN_DESCRIPTOR_MANAGER_HPP
#define VULKAN_DESCRIPTOR_MANAGER_HPP

#include "vulkan/vulkan.hpp"
#include <memory>
namespace myrender{
    class DescriptorManager final{
    public:
        static DescriptorManager& Instance();
        ~DescriptorManager();
        struct poolInfo{
            vk::DescriptorPool pool;
            int max_sets;
            int remain_sets;
            vk::DescriptorType type;
        };
        struct setInfo{
            vk::DescriptorPool pool;
            vk::DescriptorSet set;
        };
        setInfo AllocSet(vk::DescriptorType type);
        static void Init(int MAX_FRAME_SIZE);
        static void Quit();

    private:
        int MAX_FRAME_SIZE;
        DescriptorManager(int MAX_FRAME_SIZE);
        std::vector<poolInfo> descPools;
        void CreateDescriptorPool(vk::DescriptorType type);
        static std::unique_ptr<DescriptorManager> instance;




    };


}

#endif //VULKAN_DESCRIPTOR_MANAGER_HPP
