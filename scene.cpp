//
// Created by 87549 on 2023/3/7.
//

#include "scene.hpp"
namespace myrender{

    void Scene::AddObj(myrender::Object *obj) {
        scene.push_back(obj);

    }
    void Scene::DeleteObj(myrender::Object *obj) {
        for(auto r = scene.begin(); r!=scene.end(); r++){
            if(*r == obj){
                delete *r;
                scene.erase(r);
                break;
            }
        }
    }
    Scene::~Scene() {
        for(auto r = scene.begin(); r!=scene.end(); r++){
            delete *r;
        }
    }

}