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
        CreateDepthBuffer();
        GetImage();
        CreateImageViews();
    }

    Swapchain::~Swapchain() {
        auto& device = Context::GetInstance().device;
        for(auto& framebuffer:framebuffers){
           device.destroyFramebuffer(framebuffer);
        }
        for(auto& view:imageViews){
            device.destroyImageView(view);
        }
        device.destroyImageView(depthImageView);
        device.freeMemory(depthImageMemory);
        device.destroyImage(depthImage);

        auto& ctx = Context::GetInstance();
        device.destroySwapchainKHR(swapchain);
    }
    void Swapchain::CreateDepthImage() {
        uint32_t w = info.imageExtent.width;
        uint32_t h = info.imageExtent.height;
        vk::ImageCreateInfo createInfo;
        createInfo.setImageType(vk::ImageType::e2D)
                .setArrayLayers(1)
                .setMipLevels(1)
                .setExtent({w,h,1})
                .setFormat(vk::Format::eD32Sfloat)
                .setTiling(vk::ImageTiling::eOptimal)
                .setInitialLayout(vk::ImageLayout::eUndefined)
                .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment|vk::ImageUsageFlagBits::eSampled)
                .setSamples(vk::SampleCountFlagBits::e1);
        depthImage = Context::GetInstance().device.createImage(createInfo);
    }
    void Swapchain::CreateDepthImageView() {
        vk::ImageViewCreateInfo createInfo;
        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange range;
        range.setAspectMask(vk::ImageAspectFlagBits::eDepth)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setLevelCount(1)
                .setBaseMipLevel(0);

        createInfo.setImage(depthImage)
                .setViewType(vk::ImageViewType::e2D)
                .setFormat(vk::Format::eD32Sfloat)
                .setComponents(mapping)
                .setSubresourceRange(range);
        depthImageView = Context::GetInstance().device.createImageView(createInfo);
    }
    void Swapchain::AllocDepthImageMemory() {
        auto& device = Context::GetInstance().device;
        vk::MemoryAllocateInfo allocateInfo;
        auto requirements = device.getImageMemoryRequirements(depthImage);
        allocateInfo.setAllocationSize(requirements.size);
        auto index = QueryBufferMemTypeIndex(requirements.memoryTypeBits,vk::MemoryPropertyFlagBits::eDeviceLocal);
        allocateInfo.setMemoryTypeIndex(index);
        depthImageMemory = device.allocateMemory(allocateInfo);

        Context::GetInstance().device.bindImageMemory(depthImage,depthImageMemory,0);
    }
    void Swapchain::CreateDepthBuffer() {
        CreateDepthImage();
        AllocDepthImageMemory();
        CreateDepthImageView();


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
    void Swapchain::CreateFramebuffers() {
        framebuffers.resize(images.size());
        for(int i = 0; i<framebuffers.size();i++ ){
            vk::FramebufferCreateInfo createInfo;
            std::array<vk::ImageView,2> attachments={imageViews[i],depthImageView};
            createInfo.setAttachments(attachments)
            .setWidth(info.imageExtent.width)
            .setHeight(info.imageExtent.height)
            .setRenderPass(Context::GetInstance().renderProcess->renderPass)
            .setLayers(1);
            framebuffers[i] = Context::GetInstance().device.createFramebuffer(createInfo);
        }
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