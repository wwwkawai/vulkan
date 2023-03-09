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

        Object(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
        Object(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::string_view filename);
        ~Object();
    };


}




#endif //VULKAN_OBJECT_HPP
