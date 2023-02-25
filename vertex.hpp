//
// Created by 87549 on 2023/2/25.
//

#ifndef VULKAN_VERTEX_HPP
#define VULKAN_VERTEX_HPP

#include "vulkan/vulkan.hpp"

struct Vec2f final {
    float x, y;
    Vec2f(float x, float y){
        this->x = x;
        this->y = y;
    }
    static vk::VertexInputAttributeDescription GetAttriDesc(){
        vk::VertexInputAttributeDescription attr;
        attr.setBinding(0)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setLocation(0)
        .setOffset(0);
        return attr;
    }
    static vk::VertexInputBindingDescription GetBindDesc(){
        vk::VertexInputBindingDescription bind;
        bind.setBinding(0)
        .setInputRate(vk::VertexInputRate::eVertex)
        .setStride(sizeof(Vec2f));
        return bind;
    }
};




#endif //VULKAN_VERTEX_HPP
