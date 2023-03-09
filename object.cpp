//
// Created by 87549 on 2023/3/7.
//

#include "object.hpp"

namespace myrender{
    Vertex::Vertex(glm::vec3 pos, glm::vec3 color = glm::vec3(1.0), glm::vec2 textureCoord = glm::vec2(0.0)) {
        this->color = color;
        this->pos = pos;
        this->textureCoord = textureCoord;
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
    Object::Object(std::vector<Vertex> vertices, std::vector<uint16_t> indices) {
        this->vertices = vertices;
        this->indices = indices;
        texture = nullptr;
    }
    Object::Object(std::vector<Vertex> vertices, std::vector<uint16_t> indices, std::string_view filename) {
        this->vertices = vertices;
        this->indices = indices;
        texture = new Texture(filename);
    }
    Object::~Object() {
        if(texture)
            delete texture;
    }
}