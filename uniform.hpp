//
// Created by 87549 on 2023/2/26.
//

#ifndef VULKAN_UNIFORM_HPP
#define VULKAN_UNIFORM_HPP

#include "vulkan/vulkan.hpp"
namespace myrender {
    struct Color final{
        float r,g,b;
    };
    struct Uniform final {
        Color color;
        static vk::DescriptorSetLayoutBinding GetBinding(){
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eFragment);
            return binding;
        }
    };
}



#endif //VULKAN_UNIFORM_HPP
