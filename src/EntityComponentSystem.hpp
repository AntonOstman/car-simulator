#pragma once

#include "Model.hpp"
#include "glm/ext/matrix_float3x3.hpp"
#include "glm/ext/vector_float3.hpp"
#include <cstdint>
#include <unordered_set>
#include <memory>
#include <unordered_map>
#include "glad/glad.h"

using EntityID = std::int32_t;
using CompID = std::int32_t;
using StorageID = std::int32_t;

template<typename T>
struct CompId
{
    size_t idx;
    CompId() : idx(SIZE_MAX) {}

    explicit CompId(size_t index) : idx(index) {}

    bool is_valid() const
    {
        return idx != SIZE_MAX;
    };

    operator size_t() const { return idx; }
    
    bool operator==(const CompId<T>& other) const { 
        return idx == other.idx; 
    }
    bool operator!=(const CompId<T>& other) const { 
        return idx != other.idx; 
    }
};

struct IComponentStore
{
    std::unordered_map<EntityID, size_t> entityToIdx;
    std::unordered_map<size_t, EntityID> idxToEntity;
    std::unordered_set<EntityID> entities;

    void removeEntity(EntityID entity)
    {
        idxToEntity.erase(entityToIdx[entity]);
        entityToIdx.erase(entity);
        entities.erase(entity);
    }
};

template<typename T>
struct ComponentStore : IComponentStore
{
    std::vector<T> data;

    CompId<T> createComponent(T component)
    {
        assert(data.size() < SIZE_MAX && "Max size_t reached");
        CompId<T> idx = CompId<T>{data.size()};
        data.push_back(component);
        return idx;
    }

    void addComponent(EntityID entity, CompId<T> idx)
    {
        // assert(entityToIdx.count(entity) > 0 && "Entity does not have component");
        // assert(idxToEntity.count(idx) > 0 && "Entity does not have component");
        entityToIdx[entity] = idx;
        idxToEntity[idx.idx] = entity;
        entities.insert(entity);
    }

    T& getComponent(EntityID entity)
    {
        assert(entityToIdx.count(entity) > 0 && "Entity does not have component");
        return data[entityToIdx[entity]];
    }
};
struct ComponentMarker {};

struct ShaderComp : ComponentMarker{
    GLuint program;
};

struct CameraComp: ComponentMarker{
    glm::mat4 view;
    glm::mat4 perspective;
};

struct Phys: ComponentMarker{
    float mass;
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    glm::vec3 force;
    glm::mat3 inertia;
};

struct Mesh : ComponentMarker{
    GLuint VBO;
    GLuint VAO;
    unsigned int num_vert;
};

struct Transform : ComponentMarker{
    glm::mat4 modelToWorld;
};

struct Rotator : ComponentMarker{
    glm::vec3 axis;
};

class PhysicsSystem : ComponentMarker{
    public:
        void static apply_force(Phys phys, glm::vec3 f);
        void static time_step(Phys phys, float t);
};


class ECS
{
public:
    ECS();
    void init();
    void updateEntities();
    EntityID createEntity();

    void removeEntity(EntityID entity);
    EntityID getEntityWithTag(std::string tag);
    void addTag(EntityID entity, std::string tag);

    template<typename... Components>
    std::vector<EntityID> intersection_entity_id();
    template<typename T>
    T& getComponent(EntityID entity)
    {
        return getStorage<T>().getComponent(entity);
    }

    template<typename T>
    void addComponent(EntityID entity, CompId<T> idx)
    {
        getStorage<T>().addComponent(entity, idx);
    }

    template<typename T>
    CompId<T> createComponent(T component)
    {    
        return getStorage<T>().createComponent(component);
    }

private:
    template<typename T>
    ComponentStore<T>& getStorage();
public:
private:
    std::unordered_map<std::string, EntityID> _tags;
    std::vector<IComponentStore*> _storages;
    std::vector<EntityID> _entities;
};

class RenderingSystem{
    public:
        CompId<Mesh> static createMesh(const std::vector<Vertex> &vertices, ECS& ecs);
        CompId<ShaderComp> static createShader(std::string frag, std::string vert, ECS& ecs);
        void static update(ECS& ecs);

        void static setUniforms(const GLuint& program, const glm::mat4& modelp, const glm::mat4& view, const glm::mat4& projection);
        void static drawLines(Mesh& mesh, GLuint program);
        void static drawTriangles(Mesh& mesh, GLuint program);
        void static init();
    private:
    public:
    private:
};
