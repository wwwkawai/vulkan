//
// Created by 87549 on 2023/3/3.
//
#include "context.hpp"
#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace myrender{

    Texture::Texture(std::string_view filename) {
        int w,h,channel;
        stbi_uc* pixels = stbi_load(filename.data(),&w,&h,&channel,STBI_rgb_alpha);
        size_t size = w*h*4;
        if(!pixels){
            throw std::runtime_error("Load texture failed");
        }
        std::unique_ptr<Buffer> buffer(new Buffer(size,vk::BufferUsageFlagBits::eTransferSrc,vk::MemoryPropertyFlagBits::eHostCoherent|vk::MemoryPropertyFlagBits::eHostVisible));
        memcpy(buffer->ptr,pixels,size);
        CreateImage(w,h);
        AllocateMemory();
        Context::GetInstance().device.bindImageMemory(image,memory,0);
        TransitionImageLayoutFromUndefine2Dst();
        TransformData2Image(*buffer,w,h);
        TransitionImageLayoutFromDst2Optimal();
        CreateImageView();
        CreateSampler();
        UpdateDescriptorSets();
        stbi_image_free(pixels);
    }
    Texture::~Texture() {
        auto& device = Context::GetInstance().device;
        device.destroySampler(sampler);
        device.destroyImageView(imageView);
        device.freeMemory(memory);
        device.destroyImage(image);
    }

    void Texture::CreateImage(uint32_t w, uint32_t h) {
        vk::ImageCreateInfo createInfo;
        createInfo.setImageType(vk::ImageType::e2D)
        .setArrayLayers(1)
        .setMipLevels(1)
        .setExtent({w,h,1})
        .setFormat(vk::Format::eR8G8B8A8Srgb)
        .setTiling(vk::ImageTiling::eOptimal)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(vk::ImageUsageFlagBits::eTransferDst|vk::ImageUsageFlagBits::eSampled)
        .setSamples(vk::SampleCountFlagBits::e1);
        image = Context::GetInstance().device.createImage(createInfo);

    }

    void Texture::AllocateMemory() {
        auto& device = Context::GetInstance().device;
        vk::MemoryAllocateInfo allocateInfo;
        auto requirements = device.getImageMemoryRequirements(image);
        allocateInfo.setAllocationSize(requirements.size);
        auto index = QueryBufferMemTypeIndex(requirements.memoryTypeBits,vk::MemoryPropertyFlagBits::eDeviceLocal);
        allocateInfo.setMemoryTypeIndex(index);
        memory = device.allocateMemory(allocateInfo);

    }
    void Texture::CreateSampler() {
        vk::SamplerCreateInfo createInfo;
        createInfo.setMagFilter(vk::Filter::eLinear)
                .setMinFilter(vk::Filter::eLinear)
                .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                .setAnisotropyEnable(false)
                .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                .setUnnormalizedCoordinates(false)
                .setCompareEnable(false);
        sampler = Context::GetInstance().device.createSampler(createInfo);
    }
    void Texture::CreateImageView() {
        vk::ImageViewCreateInfo createInfo;
        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange range;
        range.setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setLevelCount(1)
                .setBaseMipLevel(0);

        createInfo.setImage(image)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(vk::Format::eR8G8B8A8Srgb)
                .setComponents(mapping)
                .setSubresourceRange(range);
        imageView = Context::GetInstance().device.createImageView(createInfo);
    }
    void Texture::TransitionImageLayoutFromUndefine2Dst() {
        Context::GetInstance().commandManager->ExecuteCmd(Context::GetInstance().graphicsQueue,[&](vk::CommandBuffer cmdBuf) {
            vk::ImageMemoryBarrier barrier;
            vk::ImageSubresourceRange range;
            range.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setLevelCount(1)
                    .setBaseMipLevel(0);
            barrier.setSubresourceRange(range)
            .setImage(image)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstAccessMask((vk::AccessFlagBits::eTransferWrite));
            cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe,vk::PipelineStageFlagBits::eTransfer,{},{},nullptr,barrier);


        });

    }
    void Texture::UpdateDescriptorSets() {
        descSet = DescriptorManager::Instance().AllocSet(vk::DescriptorType::eCombinedImageSampler);
        vk::DescriptorImageInfo imageInfo;
        imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setImageView(imageView)
                .setSampler(sampler);
        vk::WriteDescriptorSet writer;
        writer.setImageInfo(imageInfo)
                .setDstBinding(0)
                .setDstArrayElement(0)
                .setDescriptorCount(1)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setDstSet(descSet.set);
        Context::GetInstance().device.updateDescriptorSets(writer,{});

    }
    void Texture::TransitionImageLayoutFromDst2Optimal() {
        Context::GetInstance().commandManager->ExecuteCmd(Context::GetInstance().graphicsQueue,[&](vk::CommandBuffer cmdBuf) {
            vk::ImageMemoryBarrier barrier;
            vk::ImageSubresourceRange range;
            range.setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1)
                    .setLevelCount(1)
                    .setBaseMipLevel(0);
            barrier.setSubresourceRange(range)
                    .setImage(image)
                    .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
                    .setSrcAccessMask((vk::AccessFlagBits::eTransferWrite))
                    .setDstAccessMask((vk::AccessFlagBits::eShaderRead));
            cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,vk::PipelineStageFlagBits::eFragmentShader,{},{},nullptr,barrier);


        });
    }
    void Texture::TransformData2Image(myrender::Buffer &buffer, uint32_t w, uint32_t h) {
        Context::GetInstance().commandManager->ExecuteCmd(Context::GetInstance().graphicsQueue,[&](vk::CommandBuffer cmdBuf){
            vk::BufferImageCopy region;
            vk::ImageSubresourceLayers subresourceLayers;
            subresourceLayers.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setLayerCount(1)
            .setBaseArrayLayer(0)
            .setMipLevel(0);
            region.setBufferImageHeight(0)
            .setBufferRowLength(0)
            .setImageOffset(0)
            .setImageExtent({w,h,1})
            .setImageSubresource(subresourceLayers)
            .setBufferOffset(0);
            cmdBuf.copyBufferToImage(buffer.buffer,image,vk::ImageLayout::eTransferDstOptimal,region);

        });
    }

}
