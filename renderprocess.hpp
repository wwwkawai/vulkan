//
// Created by 87549 on 2023/2/18.
//

#ifndef VULKAN_RENDERPROCESS_HPP
#define VULKAN_RENDERPROCESS_HPP
#include "vulkan/vulkan.hpp"
namespace myrender {
    class RenderProcess final {
    public:
        vk::Pipeline pipeline;
        vk::PipelineLayout layout;
        vk::DescriptorSetLayout setLayout;
        vk::RenderPass renderPass;

        RenderProcess();
        ~RenderProcess();
        void InitSetLayout();
        void InitLayout();
        void InitRenderPass();
        void InitpPipeline(int w, int h);
    };
}


#endif //VULKAN_RENDERPROCESS_HPP
