//
// Created by 87549 on 2023/2/18.
//

#include "shader.hpp"
#include "context.hpp"
#include "glm/glm.hpp"
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
        InitStages();

    }
    Shader::~Shader() {
        auto& device = Context::GetInstance().device;
        device.destroyShaderModule(vertexModule);
        device.destroyShaderModule(fragModule);

    }
    vk::PushConstantRange Shader::GetPushConstantRange() {
        vk::PushConstantRange range;
        range.setOffset(0)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex)
        .setSize(sizeof(glm::mat4));
        return range;
    }
    void Shader::InitStages() {
        _stages.resize(2);
        _stages[0].setStage(vk::ShaderStageFlagBits::eVertex)
                .setModule(vertexModule)
                .setPName("main");

        _stages[1].setStage(vk::ShaderStageFlagBits::eFragment)
                .setModule(fragModule)
                .setPName("main");

    }
    std::vector<vk::PipelineShaderStageCreateInfo> Shader::GetStages() {
        return _stages;
    }
}