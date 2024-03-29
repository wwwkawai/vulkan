//
// Created by 87549 on 2023/2/18.
//

#include "renderprocess.hpp"
#include "shader.hpp"
#include "context.hpp"
#include "object.hpp"
#include "uniform.hpp"
namespace myrender{
    RenderProcess::RenderProcess() {
        InitSetLayout();
        InitLayout();
        InitRenderPass();
        pipeline = nullptr;
    }
    void RenderProcess::InitpPipeline(int width, int height) {
        vk::GraphicsPipelineCreateInfo createInfo;
        //vertext input
        vk::PipelineVertexInputStateCreateInfo inputState;
        auto attr = Vertex::GetAttriDesc();
        auto bind = Vertex::GetBindDesc();
        inputState.setVertexAttributeDescriptions(attr)
        .setVertexBindingDescriptions(bind);
        createInfo.setPVertexInputState(&inputState);
        //vertex assembly
        vk::PipelineInputAssemblyStateCreateInfo inputAsm;
        inputAsm.setPrimitiveRestartEnable(false)
        .setTopology(vk::PrimitiveTopology::eTriangleList);
        createInfo.setPInputAssemblyState(&inputAsm);
        //shader
        auto stages = Shader::GetInstance().GetStages();
        createInfo.setStages(stages);

        //viewport transform
        vk::PipelineViewportStateCreateInfo viewportState;
        vk::Viewport viewport(0,0,width,height,0,1);
        vk::Rect2D rect({0,0},{static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
        viewportState.setViewports(viewport)
        .setScissors(rect);
        createInfo.setPViewportState(&viewportState);


        //Rasterization
        vk::PipelineRasterizationStateCreateInfo rastInfo;
        rastInfo.setRasterizerDiscardEnable(false)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1);
        createInfo.setPRasterizationState(&rastInfo);

        //Multi-sample
        vk::PipelineMultisampleStateCreateInfo multisample;
        multisample.setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);
        createInfo.setPMultisampleState(&multisample);

        //Test stencil/depth
        vk::PipelineDepthStencilStateCreateInfo depthStencil;
        depthStencil.setDepthTestEnable(true)
        .setDepthWriteEnable(true)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(false)
        .setStencilTestEnable(false);
        createInfo.setPDepthStencilState(&depthStencil);

        //Color blending
        /*newRGB = srcRGB * srcFactor <op> dstFactor * dstRGB
         *newAlpha = srcAlpha * srcFactor <op> dstFactor * dstAlpha
         *
         *newRGB = srcRGB * 1 + (1-srcAlpha) * dstRGB
         * newAlpha = srcAlpha * 1 + 0 * dstAlpha
         */
        vk::PipelineColorBlendStateCreateInfo blend;
        vk::PipelineColorBlendAttachmentState attachs;
        attachs.setBlendEnable(true)
        .setColorWriteMask(vk::ColorComponentFlagBits::eA
        |vk::ColorComponentFlagBits::eB
        |vk::ColorComponentFlagBits::eG
        |vk::ColorComponentFlagBits::eR)
        .setSrcColorBlendFactor(vk::BlendFactor::eOne)
        .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
        .setColorBlendOp(vk::BlendOp::eAdd)
        .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
        .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
        .setAlphaBlendOp(vk::BlendOp::eAdd);
        blend.setLogicOpEnable(false)
        .setAttachments(attachs)
        .setLogicOp(vk::LogicOp::eOrReverse);
        createInfo.setPColorBlendState(&blend);

        //RenderPass and Layout
        createInfo.setRenderPass(renderPass)
        .setLayout(layout);


        auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr,createInfo);
        if(result.result!=vk::Result::eSuccess){
            throw std::runtime_error("Create pipeline failed");
        }
        pipeline = result.value;



    }
    void RenderProcess::InitSetLayout() {
        vk::DescriptorSetLayoutCreateInfo createInfo;
        auto& device =Context::GetInstance().device;
        std::vector<vk::DescriptorSetLayoutBinding> bindings(2);
        bindings[0] = Uniform::GetBinding();
        createInfo.setBindings(bindings[0]);
        setLayout.push_back(device.createDescriptorSetLayout(createInfo));
        bindings[1].setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);
        createInfo.setBindings(bindings[1]);
        setLayout.push_back(device.createDescriptorSetLayout(createInfo));
    }
    void RenderProcess::InitLayout() {
        vk::PipelineLayoutCreateInfo createInfo;
        auto range = Shader::GetInstance().GetPushConstantRange();
        createInfo.setSetLayouts(setLayout)
        .setPushConstantRanges(range);
        layout = Context::GetInstance().device.createPipelineLayout(createInfo);

    }
    void RenderProcess::InitRenderPass() {
        vk::RenderPassCreateInfo createInfo;
        vk::AttachmentDescription attachmentDescription;
        attachmentDescription.setFormat(Context::GetInstance().swapchain->info.format.format)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setSamples(vk::SampleCountFlagBits::e1);

        vk::AttachmentDescription depthAttachment;
        depthAttachment.setFormat(vk::Format::eD32Sfloat)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);


        std::array<vk::AttachmentDescription,2> attachments = {attachmentDescription,depthAttachment};
        createInfo.setAttachments(attachments);
        vk::AttachmentReference attachmentReference, depthReference;
        attachmentReference.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setAttachment(0);
        depthReference.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setAttachment(1);

        vk::SubpassDescription subpassDescription;
        subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachments(attachmentReference)
        .setPDepthStencilAttachment(&depthReference)
        .setColorAttachmentCount(1);
        createInfo.setSubpasses(subpassDescription);

        vk::SubpassDependency dependency;
        dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite|vk::AccessFlagBits::eDepthStencilAttachmentWrite)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput|vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput|vk::PipelineStageFlagBits::eEarlyFragmentTests);
        createInfo.setDependencies(dependency);
        renderPass = Context::GetInstance().device.createRenderPass(createInfo);
    }
    RenderProcess::~RenderProcess() {
        auto& device = Context::GetInstance().device;
        for(int i=0;i<setLayout.size();i++)
            device.destroyDescriptorSetLayout(setLayout[i]);
        device.destroyRenderPass(renderPass);
        device.destroyPipelineLayout(layout);
        device.destroyPipeline(pipeline);
    }
}
