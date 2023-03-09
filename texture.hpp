//
// Created by 87549 on 2023/3/3.
//

#ifndef VULKAN_TEXTURE_HPP
#define VULKAN_TEXTURE_HPP
#include "vulkan/vulkan.hpp"
#include "buffer.hpp"
#include <string_view>
#include "descriptor_manager.hpp"

namespace myrender{
    class Texture final{
    public:
        Texture(std::string_view filename);
        ~Texture();
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView imageView;
        void UpdateDescriptorSets();
        vk::Sampler sampler;
        DescriptorManager::setInfo descSet;

    private:
        void CreateImage(uint32_t w, uint32_t h);
        void AllocateMemory();
        void CreateImageView();
        void CreateSampler();
        uint32_t QueryImageMemoryIndex();
        void TransitionImageLayoutFromUndefine2Dst();
        void TransitionImageLayoutFromDst2Optimal();
        void TransformData2Image(Buffer& buffer, uint32_t w, uint32_t h);
    };
}



#endif //VULKAN_TEXTURE_HPP
