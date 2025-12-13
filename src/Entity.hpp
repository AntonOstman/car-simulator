#pragma once

#include "Math.hpp"
#include "RenderingSystem.hpp"

struct Phys{
    float mass;
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    glm::vec3 force;
};

struct AABB {
    glm::vec3 max;
    glm::vec3 min;

    bool isInside(glm::vec3 other)
    {
        bool maxCheck = (other.x < max.x) && (other.y < max.y) && (other.z < max.z);
        bool minCheck = (other.x > min.x) && (other.y > min.y) && (other.z > min.z);
        return minCheck && maxCheck;
    }
};

struct Transform{
    glm::mat4 modelToWorld;

    void setTranslation(glm::vec3 trans){
        modelToWorld[3] = glm::vec4(trans, 1.0);
    }

    glm::vec3 getTranslation(){
        return glm::vec3(modelToWorld[3]);
    }

    glm::vec3 getSize(){
        return glm::vec3(modelToWorld[0][0], modelToWorld[1][1], modelToWorld[2][2]) * 2.f;
    }

    AABB getAABB(){
        AABB box;
        glm::vec3 size = getSize();
        glm::vec3 center = getTranslation();

        box.min = center - size / 2.f;
        box.max = center + size / 2.f;
        return box;
    }

    void init(float scale){
        modelToWorld = scaled_eye(scale);
    }
};

class Entity
{
    public:
        Entity(MeshType mesh, ShaderType shader);
        MeshType _mesh;
        ShaderType _shader;
        Phys _phys;
        Transform _transform;
        float _size;
        void setPosition(glm::vec3 pos);
        void render(glm::mat4 worldToView, glm::mat4 projection);
    private:
        void update();
};
