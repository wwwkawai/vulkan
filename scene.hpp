//
// Created by 87549 on 2023/3/7.
//

#ifndef VULKAN_SCENE_HPP
#define VULKAN_SCENE_HPP
#include "object.hpp"
#include <list>
namespace myrender{
    class Scene final{
    public:
        ~Scene();
        std::list<Object*> scene;
        void AddObj(Object* obj);
        void DeleteObj(Object* obj);
    };
}
#endif //VULKAN_SCENE_HPP
