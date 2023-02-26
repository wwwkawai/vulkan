//
// Created by 87549 on 2023/2/13.
//

#include "context.hpp"

namespace myrender {
    std::unique_ptr<Context> Context::_instance = nullptr;

    void Context::Init(const std::vector<const char *> &extensions, CreateSurfaceFunc func) {
        _instance.reset(new Context(extensions, func));
    }

    void Context::Quit() {
        _instance.reset();
    }

    Context &Context::GetInstance() {
        return *_instance;
    }

    Context::Context(const std::vector<const char *> &extensions, CreateSurfaceFunc func) {
        CreateInstance(extensions);
        SelectPhysicalDevice();
        surface = func(instance);
        QueryQueueFamilyIndices();
        CreateDevice();
        GetQueues();
    }


    Context::~Context() {
        instance.destroySurfaceKHR(surface);
        device.destroy();
        instance.destroy();
    }

    void Context::CreateInstance(const std::vector<const char *> &extensions) {
        vk::InstanceCreateInfo createInfo;
        std::vector<const char *> layers = {"VK_LAYER_KHRONOS_validation"};
        vk::ApplicationInfo appInfo;
        appInfo.setApiVersion(VK_API_VERSION_1_3);
        createInfo.setPApplicationInfo(&appInfo);
        createInfo.setPEnabledLayerNames(layers)
                .setPEnabledExtensionNames(extensions);
        instance = vk::createInstance(createInfo);
    }

    void Context::SelectPhysicalDevice() {
        auto devices = instance.enumeratePhysicalDevices();
        phyDevice = devices[0];
        std::cout << phyDevice.getProperties().deviceName << std::endl;
    }



    void Context::CreateDevice() {
        std::array extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        vk::DeviceCreateInfo createInfo;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        float priority = 1.0;
        if (queueFamilyIndices.presentQueue.value() == queueFamilyIndices.graphicsQueue.value()) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setPQueuePriorities(&priority)
                    .setQueueCount(1)
                    .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
            queueCreateInfos.push_back(queueCreateInfo);
        } else {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setPQueuePriorities(&priority)
                    .setQueueCount(1)
                    .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
            queueCreateInfos.push_back(queueCreateInfo);
            queueCreateInfo.setPQueuePriorities(&priority)
                    .setQueueCount(1)
                    .setQueueFamilyIndex(queueFamilyIndices.presentQueue.value());
            queueCreateInfos.push_back(queueCreateInfo);

        }
        createInfo.setQueueCreateInfos(queueCreateInfos)
        .setPEnabledExtensionNames(extensions);
        device = phyDevice.createDevice(createInfo);
    }

    void Context::QueryQueueFamilyIndices() {
        auto properties = phyDevice.getQueueFamilyProperties();
        for (int i = 0; i < properties.size(); i++) {
            auto &property = properties[i];
            if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
                queueFamilyIndices.graphicsQueue = i;
            }
            if (phyDevice.getSurfaceSupportKHR(i, surface)) {
                queueFamilyIndices.presentQueue = i;
            }
            if (queueFamilyIndices) {
                break;
            }
        }
    }

    void Context::GetQueues() {
        graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
        presentQueue = device.getQueue(queueFamilyIndices.presentQueue.value(), 0);
    }
}