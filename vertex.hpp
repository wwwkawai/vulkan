//
// Created by 87549 on 2023/2/25.
//

#ifndef VULKAN_VERTEX_HPP
#define VULKAN_VERTEX_HPP

#include "vulkan/vulkan.hpp"
namespace myrender {

    struct Color {
        float r, g, b;

        Color(float r, float g, float b) : r(r), g(g), b(b) {}
    };

    struct Vec2f final {
        float x, y;

        Vec2f(float x, float y) {
            this->x = x;
            this->y = y;
        }

        static vk::VertexInputAttributeDescription GetAttriDesc() {
            vk::VertexInputAttributeDescription attr;
            attr.setBinding(0)
                    .setFormat(vk::Format::eR32G32Sfloat)
                    .setLocation(0)
                    .setOffset(0);
            return attr;
        }

        static vk::VertexInputBindingDescription GetBindDesc() {
            vk::VertexInputBindingDescription bind;
            bind.setBinding(0)
                    .setInputRate(vk::VertexInputRate::eVertex)
                    .setStride(sizeof(Vec2f));
            return bind;
        }
    };
    class Vertex final{
    public:
        Vec2f pos;
        Color color;
        Vec2f texcoord;
        Vertex(float x, float y, float r, float g, float b, float u, float v):pos(x,y), color(r,g,b), texcoord(u,v){}
        static std::vector<vk::VertexInputAttributeDescription> GetAttriDesc() {
            std::vector<vk::VertexInputAttributeDescription> attr(3);
            attr[0].setBinding(0)
                    .setFormat(vk::Format::eR32G32Sfloat)
                    .setLocation(0)
                    .setOffset(offsetof(Vertex,pos));
            attr[1].setBinding(0)
                    .setFormat(vk::Format::eR32G32B32Sfloat)
                    .setLocation(1)
                    .setOffset(offsetof(Vertex,color));
            attr[2].setBinding(0)
                    .setFormat(vk::Format::eR32G32Sfloat)
                    .setLocation(2)
                    .setOffset(offsetof(Vertex,texcoord));
            return attr;
        }

        static vk::VertexInputBindingDescription GetBindDesc() {
            vk::VertexInputBindingDescription bind;
            bind.setBinding(0)
                    .setInputRate(vk::VertexInputRate::eVertex)
                    .setStride(sizeof(Vertex));
            return bind;
        }
    };
}

#endif //VULKAN_VERTEX_HPP
