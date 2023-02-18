//
// Created by 87549 on 2023/2/16.
//

#ifndef VULKAN_TOOL_HPP
#define VULKAN_TOOL_HPP
#include "vulkan/vulkan.hpp"
#include "functional"
#include "vector"
#include <iostream>
#include <fstream>
namespace myrender{
    using CreateSurfaceFunc = std::function<vk::SurfaceKHR(vk::Instance)>;

    std::string ReadWholeFile(const std::string& filename);

}
#endif //VULKAN_TOOL_HPP
