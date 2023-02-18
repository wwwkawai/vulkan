//
// Created by 87549 on 2023/2/18.
//

#ifndef VULKAN_SHADER_HPP
#define VULKAN_SHADER_HPP
#include "vulkan/vulkan.hpp"
#include <memory>

namespace myrender{
    class Shader final {
    public:
        static void Init(const std::string& vertexSource, const std::string& fragSource);
        static void Quit();
        static Shader& GetInstance();
        vk::ShaderModule vertexModule;
        vk::ShaderModule fragModule;
        ~Shader();
    private:
        static std::unique_ptr<Shader> _instance;
        Shader(const std::string& vertexSource, const std::string& fragSource);

    };
}
#endif //VULKAN_SHADER_HPP
