//
// Created by 87549 on 2023/3/7.
//
#ifndef VULKAN_OBJECT_HPP
#define VULKAN_OBJECT_HPP
#include <vector>
#include "glm/glm.hpp"
#include "texture.hpp"

namespace myrender{

    class Vertex{
    public:
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 textureCoord;
        Vertex(glm::vec3 pos, glm::vec3 color, glm::vec2 textureCoord);
        static std::vector<vk::VertexInputAttributeDescription> GetAttriDesc();
        static vk::VertexInputBindingDescription GetBindDesc();
    };

    class Object{
    public:
        std::vector<Vertex> vertices;
        Texture* texture;
        std::vector<uint16_t> indices;
        std::unique_ptr<Buffer> hostVertexBuf;
        std::unique_ptr<Buffer> deviceVertexBuf;
        std::unique_ptr<Buffer> hostIndicesBuf;
        std::unique_ptr<Buffer> deviceIndicesBuf;

        Object(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
        Object(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::string_view filename);
        ~Object();
        void CreateVerticesBuf();
        void BufVerticesData();
        void CreateIndicesBuf();
        void BufIndicesData();
        void CopyFromBuf(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);


    };


}




#endif //VULKAN_OBJECT_HPP
