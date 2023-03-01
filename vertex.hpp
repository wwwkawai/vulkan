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
        Vertex(float x, float y, float r, float g, float b):pos(x,y), color(r,g,b){}
        static std::vector<vk::VertexInputAttributeDescription> GetAttriDesc() {
            std::vector<vk::VertexInputAttributeDescription> attr(2);
            attr[0].setBinding(0)
                    .setFormat(vk::Format::eR32G32Sfloat)
                    .setLocation(0)
                    .setOffset(offsetof(Vertex,pos));
            attr[1].setBinding(0)
                    .setFormat(vk::Format::eR32G32B32Sfloat)
                    .setLocation(1)
                    .setOffset(offsetof(Vertex,color));
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


    struct Vec {
        union {
            struct {
                float x, y;
            };
            struct {
                float w, h;
            };
        };

        static vk::VertexInputAttributeDescription GetAttributeDescription();

        static vk::VertexInputBindingDescription GetBindingDescription();
    };


    class Mat4 {
    public:
        static Mat4 CreateIdentity();

        static Mat4 CreateOnes();

        static Mat4 CreateOrtho(int left, int right, int top, int bottom, int near, int far);

        static Mat4 CreateTranslate(const Vec &);

        static Mat4 CreateScale(const Vec &);

        static Mat4 Create(const std::initializer_list<float> &);

        Mat4();

        const float *GetData() const { return data_; }

        void Set(int x, int y, float value) {
            data_[x * 4 + y] = value;
        }

        float Get(int x, int y) const {
            return data_[x * 4 + y];
        }

        Mat4 Mul(const Mat4 &m) const;

    private:
        float data_[4 * 4];
    };

}

#endif //VULKAN_VERTEX_HPP
