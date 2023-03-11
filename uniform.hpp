//
// Created by 87549 on 2023/2/26.
//

#ifndef VULKAN_UNIFORM_HPP
#define VULKAN_UNIFORM_HPP

#include "vulkan/vulkan.hpp"
#include "glm/glm.hpp"
namespace myrender {
    struct Uniform final {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
        static vk::DescriptorSetLayoutBinding GetBinding(){
            vk::DescriptorSetLayoutBinding binding;
            binding.setBinding(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setStageFlags(vk::ShaderStageFlagBits::eVertex);
            return binding;
        }
    };

}



#endif //VULKAN_UNIFORM_HPP
