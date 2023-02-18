//
// Created by 87549 on 2023/2/18.
//

#include "shader.hpp"
#include "context.hpp"
namespace myrender{
    std::unique_ptr<Shader> Shader::_instance = nullptr;
    void Shader::Init(const std::string &vertexSource, const std::string &fragSource) {
        _instance.reset(new Shader(vertexSource,fragSource));
    }
    void Shader::Quit() {
        _instance.reset();
    }
    Shader& Shader::GetInstance() {
        return *_instance;
    }
    Shader::Shader(const std::string &vertexSource, const std::string &fragSource) {
        vk::ShaderModuleCreateInfo createInfo;
        createInfo.codeSize = vertexSource.size();
        createInfo.pCode = (uint32_t*)vertexSource.data();
        vertexModule = Context::GetInstance().device.createShaderModule(createInfo);
        createInfo.codeSize = fragSource.size();
        createInfo.pCode = (uint32_t*)fragSource.data();
        fragModule = Context::GetInstance().device.createShaderModule(createInfo);

    }
    Shader::~Shader() {
        auto& device = Context::GetInstance().device;
        device.destroyShaderModule(vertexModule);
        device.destroyShaderModule(fragModule);

    }
}