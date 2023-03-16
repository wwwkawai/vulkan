//
// Created by 87549 on 2023/2/16.
//

#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP
#include "vulkan/vulkan.hpp"
namespace myrender{
    class Swapchain final{
    public:
        vk::SwapchainKHR swapchain;
        Swapchain(int w, int h);
        ~Swapchain();
        struct SwapchainInfo{
            vk::Extent2D imageExtent;
            uint32_t imageCount;
            vk::SurfaceFormatKHR format;
            vk::SurfaceTransformFlagBitsKHR transform;
            vk::PresentModeKHR presentMode;
        };
        vk::Image depthImage;
        vk::ImageView depthImageView;
        vk::DeviceMemory depthImageMemory;
        std::vector<vk::Image> images;
        std::vector<vk::ImageView> imageViews;
        SwapchainInfo info;
        std::vector<vk::Framebuffer> framebuffers;
        void QueryInfo(int w, int h);
        void GetImage();
        void CreateDepthBuffer();
        void CreateDepthImage();
        void CreateDepthImageView();
        void AllocDepthImageMemory();
        void CreateImageViews();
        void CreateFramebuffers();
    };


}
#endif //VULKAN_SWAPCHAIN_HPP
