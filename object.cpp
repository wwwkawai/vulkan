//
// Created by 87549 on 2023/3/7.
//

#include "object.hpp"
#include "context.hpp"
namespace myrender{
    Vertex::Vertex(glm::vec3 pos, glm::vec3 color = glm::vec3(1.0), glm::vec2 textureCoord = glm::vec2(0.0)) {
        this->color = color;
        this->pos = pos;
        this->textureCoord = textureCoord;
    }
    Vertex::Vertex() {}
    vk::VertexInputBindingDescription Vertex::GetBindDesc() {
        vk::VertexInputBindingDescription bind;
        bind.setBinding(0)
                .setInputRate(vk::VertexInputRate::eVertex)
                .setStride(sizeof(Vertex));
        return bind;
    }
    std::vector<vk::VertexInputAttributeDescription> Vertex::GetAttriDesc() {
        std::vector<vk::VertexInputAttributeDescription> attr(3);
        attr[0].setBinding(0)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setLocation(0)
                .setOffset(offsetof(Vertex,pos));
        attr[1].setBinding(0)
                .setFormat(vk::Format::eR32G32B32Sfloat)
                .setLocation(1)
                .setOffset(offsetof(Vertex,color));
        attr[2].setBinding(0)
                .setFormat(vk::Format::eR32G32Sfloat)
                .setLocation(2)
                .setOffset(offsetof(Vertex,textureCoord));
        return attr;
    }
    Object::Object(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
        this->vertices = vertices;
        this->indices = indices;
        texture = nullptr;
    }
    void Object::CreateIndicesBuf() {
        hostIndicesBuf.reset(new Buffer(sizeof(uint32_t)*indices.size(),vk::BufferUsageFlagBits::eTransferSrc,vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent));
        deviceIndicesBuf.reset(new Buffer(sizeof(uint32_t)*indices.size(),vk::BufferUsageFlagBits::eTransferDst|vk::BufferUsageFlagBits::eIndexBuffer,vk::MemoryPropertyFlagBits::eDeviceLocal));

    }
    void Object::BufIndicesData() {
        memcpy(hostIndicesBuf->ptr,indices.data(),sizeof(uint32_t)*indices.size());
        CopyFromBuf(hostIndicesBuf->buffer, deviceIndicesBuf->buffer, hostIndicesBuf->size,0,0);
    }
    void Object::CreateVerticesBuf() {
        hostVertexBuf.reset(new Buffer(sizeof(Vertex)*vertices.size(),vk::BufferUsageFlagBits::eTransferSrc,vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent));
        deviceVertexBuf.reset(new Buffer(sizeof(Vertex)*vertices.size(),vk::BufferUsageFlagBits::eTransferDst|vk::BufferUsageFlagBits::eVertexBuffer,vk::MemoryPropertyFlagBits::eDeviceLocal));

    }
    void Object::BufVerticesData() {
        memcpy(hostVertexBuf->ptr,vertices.data(),sizeof(Vertex)*vertices.size());
        CopyFromBuf(hostVertexBuf->buffer, deviceVertexBuf->buffer, hostVertexBuf->size, 0, 0);
    }
    void Object::CopyFromBuf(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset) {
        auto cmdBuf = Context::GetInstance().commandManager->CreateOneCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo); {
            vk::BufferCopy region;
            region.setSize(size)
                    .setSrcOffset(srcOffset)
                    .setDstOffset(dstOffset);
            cmdBuf.copyBuffer(src,dst,region);
        } cmdBuf.end();
        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuf);
        Context::GetInstance().graphicsQueue.submit(submitInfo);
        Context::GetInstance().device.waitIdle();
        Context::GetInstance().commandManager->FreeCmd(cmdBuf);
    }
    Object::Object(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::string_view filename) {
        this->vertices = vertices;
        this->indices = indices;
        CreateVerticesBuf();
        BufVerticesData();
        CreateIndicesBuf();
        BufIndicesData();
        texture = new Texture(filename);
    }
    Object::~Object() {
        hostIndicesBuf.reset();
        deviceIndicesBuf.reset();
        hostVertexBuf.reset();
        deviceVertexBuf.reset();
        if(texture)
            delete texture;
    }
}