//
// Created by 87549 on 2023/2/16.
//

#include "swapchain.hpp"
#include "context.hpp"
namespace myrender {
    Swapchain::Swapchain(int w, int h) {
        QueryInfo(w, h);
        vk::SwapchainCreateInfoKHR createInfo;
        createInfo.setClipped(true)
                .setImageArrayLayers(1)
                .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
                .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                .setSurface(Context::GetInstance().surface)
                .setImageColorSpace(info.format.colorSpace)
                .setImageFormat(info.format.format)
                .setImageExtent(info.imageExtent)
                .setMinImageCount(info.imageCount)
                .setPresentMode(info.presentMode)
                .setPreTransform(info.transform);
        auto& queueFamilyIndices = Context::GetInstance().queueFamilyIndices;
        if(queueFamilyIndices.presentQueue.value()==queueFamilyIndices.graphicsQueue.value()){
            createInfo.setQueueFamilyIndices(queueFamilyIndices.graphicsQueue.value())
            .setImageSharingMode(vk::SharingMode::eExclusive);
        }
        else{
            std::array indices = {queueFamilyIndices.graphicsQueue.value(),queueFamilyIndices.presentQueue.value()};
            createInfo.setQueueFamilyIndices(indices)
            .setImageSharingMode(vk::SharingMode::eConcurrent);
        }
        swapchain = Context::GetInstance().device.createSwapchainKHR(createInfo);
        GetImage();
        CreateImageViews();
    }

    Swapchain::~Swapchain() {
        for(auto& view:imageViews){
            Context::GetInstance().device.destroyImageView(view);
        }
        auto& ctx = Context::GetInstance();
        ctx.device.destroySwapchainKHR(swapchain);
    }

    void Swapchain::QueryInfo(int w, int h) {
        auto &physicDevice = Context::GetInstance().phyDevice;
        auto &surface = Context::GetInstance().surface;

        auto formats = physicDevice.getSurfaceFormatsKHR(surface);
        info.format = formats[0];
        for (const auto &format: formats) {
            if (format.format == vk::Format::eB8G8R8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                info.format = format;
                break;
            }
        }
        auto capabilities = physicDevice.getSurfaceCapabilitiesKHR(surface);
        info.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);
        info.imageExtent.width = std::clamp<uint32_t>(w,capabilities.minImageExtent.width,capabilities.maxImageExtent.width);
        info.imageExtent.height = std::clamp<uint32_t>(w,capabilities.minImageExtent.height,capabilities.maxImageExtent.height);
        info.transform = capabilities.currentTransform;
        auto presentModes = physicDevice.getSurfacePresentModesKHR(surface);
        info.presentMode = vk::PresentModeKHR::eFifo;
        for(const auto& mode:presentModes){
            if(mode == vk::PresentModeKHR::eMailbox){
                info.presentMode = mode;
                break;
            }
        }
    }
    void Swapchain::GetImage() {
        images = Context::GetInstance().device.getSwapchainImagesKHR(swapchain);
    }
    void Swapchain::CreateImageViews() {
        imageViews.resize(images.size());
        for (int i = 0; i < images.size(); i++) {
            vk::ImageViewCreateInfo createInfo;
            vk::ComponentMapping mapping;
            vk::ImageSubresourceRange range;
            range.setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setAspectMask(vk::ImageAspectFlagBits::eColor);
            createInfo.setImage(images[i])
            .setViewType(vk::ImageViewType::e2D)
            .setComponents(mapping)
            .setFormat(info.format.format)
            .setSubresourceRange(range);
            imageViews[i] = Context::GetInstance().device.createImageView(createInfo);
        }
    }

}