//
// Created by 87549 on 2023/2/13.
//

#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP
#include "vulkan/vulkan.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <optional>
#include "tool.hpp"
#include "swapchain.hpp"
#include "renderprocess.hpp"
#include "render.hpp"
#include "command.hpp"
namespace myrender{
    class Context final{
    public:
        static void Init(const std::vector<const char*>& extensions,CreateSurfaceFunc func);
        static void Quit();
        static Context& GetInstance();
        ~Context();
        vk::Instance instance;
        vk::PhysicalDevice phyDevice;
        vk::Device device;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        vk::SurfaceKHR surface;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<RenderProcess> renderProcess;
        std::unique_ptr<Render> render;
        std::unique_ptr<CommandManager> commandManager;

        struct QueueFamilyIndices final {
            std::optional<uint32_t> graphicsQueue;
            std::optional<uint32_t> presentQueue;
            operator bool() {
                return graphicsQueue.has_value() && presentQueue.has_value();
            }
        };

        QueueFamilyIndices queueFamilyIndices;
        void InitSwapchain(int w, int h){
            swapchain.reset(new Swapchain(w,h));
        }
        void InitRender(){
            render.reset(new Render);
        }
        void InitCommandManager(){
            commandManager = std::make_unique<CommandManager>();
        }
        void DestroySwapchain(){
            swapchain.reset();
        }
    private:
        Context(const std::vector<const char*>& extensions,CreateSurfaceFunc func);
        static std::unique_ptr<Context> _instance;
        void CreateInstance(const std::vector<const char*>& extensions);
        void SelectPhysicalDevice();
        void CreateDevice();
        void QueryQueueFamilyIndices();

        void GetQueues();


    };
}


#endif //VULKAN_CONTEXT_HPP
