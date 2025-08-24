#pragma once

#include "Math.hpp"
#include "glm/ext/vector_float3.hpp"
#include <atomic>
#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include "glad/glad.h"
#include <string>

using EntityID = std::int32_t;
const std::int32_t INVALID_ENTITY = -1;

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
    std::unordered_map<std::string, CompId<T>> named_component;

    // TODO: Add a way to remove components
    CompId<T> createComponent(T component)
    {
        assert(data.size() < SIZE_MAX && "Max size_t reached");
        CompId<T> idx = CompId<T>{data.size()};
        data.push_back(component);
        return idx;
    }

    CompId<T> createNamedComponent(T component, std::string name)
    {
        assert(data.size() < SIZE_MAX && "Max size_t reached");
        CompId<T> idx = CompId<T>{data.size()};
        data.push_back(component);
        named_component[name] = idx;
        return idx;
    }

    void addComponent(EntityID entity, CompId<T> idx)
    {
        entityToIdx[entity] = idx;
        idxToEntity[idx.idx] = entity;
        entities.insert(entity);
    }

    CompId<T> getNamedComponent(std::string name)
    {
        assert(named_component.count(name) > 0 && "Named component does not exist");
        return named_component[name];
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
    glm::vec2 cur_angle;
};

struct Phys: ComponentMarker{
    float mass;
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 acc;
    glm::vec3 force;
};

struct Mesh : ComponentMarker{
    GLuint VBO;
    GLuint VAO;
    unsigned int num_vert;
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

struct Transform : ComponentMarker{
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

struct Rotator : ComponentMarker{
    glm::vec3 axis;
};

class ECS
{
inline static std::atomic<int> _instance_count = 0;
static constexpr int _max_instances = 1;

public:
    ECS();

    // Copying or moving an ECS does not currently make sense
    ECS(const ECS&) = delete;
    ECS& operator=(const ECS&) = delete;
    ECS(ECS&&) = delete;
    ECS& operator=(ECS&&) = delete;

    void init();
    void updateEntities();
    EntityID createEntity();
    void removeEntity(EntityID entity);
    EntityID getEntityWithTag(std::string tag);
    void addTag(EntityID entity, std::string tag);

    template<typename... Components>
    std::vector<EntityID> intersection_entity_id() {
        std::vector<EntityID> intersections;

        for (EntityID entity : _entities) {
            if ((getStorage<Components>().entities.count(entity) && ...)) {
                intersections.push_back(entity);
            }
        }
        return intersections;
    }

    template<typename T>
    T& getComponent(EntityID entity)
    {
        assert(entity != INVALID_ENTITY && "Invalid entity"); 
        return getStorage<T>().getComponent(entity);
    }

    template<typename T>
    CompId<T> createNamedComponent(T component, std::string name)
    {
        return getStorage<T>().createNamedComponent(component, name);
    }

    template<typename T>
    CompId<T> getNamedComponent(std::string name)
    {
        return getStorage<T>().getNamedComponent(name);
    }

    template<typename T>
    void addComponent(EntityID entity, CompId<T> idx)
    {
        assert(entity != INVALID_ENTITY && "Invalid entity"); 
        getStorage<T>().addComponent(entity, idx);
    }

    template<typename T>
    CompId<T> createComponent(T component)
    {
        return getStorage<T>().createComponent(component);
    }


private:
    template<typename T>
    ComponentStore<T>& getStorage()
    {
        // Sanity check each type before creation.
        static_assert(!std::is_pointer_v<T>, "Component type cannot be a pointer.");
        static_assert(!std::is_reference_v<T>, "Component type cannot be a reference.");
        static_assert(!std::is_const_v<T>, "Component type cannot be const.");
        static_assert(!std::is_volatile_v<T>, "Component type cannot be volatile.");
        static_assert(std::is_trivially_copyable_v<T>,
                      "Component type must be trivially copyable for this ECS.");
        static_assert(!std::is_base_of_v<IComponentStore, T>,
                      "You passed a storage type instead of a component type.");

        // Make sure each component is marked as a component
        static_assert(std::is_base_of_v<ComponentMarker, T>,
                      "Component type must inherit ComponentMarker");


        static ComponentStore<T> storage = ComponentStore<T>{};
        static bool registered = false;
        if (!registered)
        {
            _storages.push_back(&storage);
            registered = true;
        }
        return storage;
    }
public:
private:
    std::unordered_map<std::string, EntityID> _tags;
    std::vector<IComponentStore*> _storages;
    std::vector<EntityID> _entities;
};

class PhysicsSystem : ComponentMarker{
    public:
        void static apply_force(Phys& phys, glm::vec3 f);
        void static time_step(Phys& phys, float t);
        void static update(ECS& ecs);
        Phys static createPhysComp();
        void static collision(Phys& phys);

        void static elastic_collision_headon(Phys& p1);
        void static elastic_collision_immovable(Phys& p1);
        void static elastic_collision(Phys& p1, Phys& p2);
};

