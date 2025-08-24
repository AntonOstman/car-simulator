#include "EntityComponentSystem.hpp"
#include "App.hpp"
#include "glad/glad.h"
#include "Debug.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <math.h>
#include "Math.hpp"
#include "Camera.hpp"

Phys PhysicsSystem::createPhysComp()
{
    Phys comp;
    comp.force = glm::vec3(0,0,0);
    comp.acc = glm::vec3(0,0,0);
    comp.pos = glm::vec3(0,10,0);
    comp.vel = glm::vec3(0,0,0);
    comp.mass = 1.0;
    return comp;
}

void PhysicsSystem::update(ECS& ecs){

    std::vector entities = ecs.intersection_entity_id<Transform, Mesh, Phys>();
    float dt = 0.1;

    glm::vec3 gravity = glm::vec3(0,-0.01,0);

    for (EntityID entity : entities)
    {
        Phys& phys = ecs.getComponent<Phys>(entity);
        phys.force = glm::vec3(0);
        Transform& trans = ecs.getComponent<Transform>(entity);
        if (phys.pos.y > 0)
        {
            phys.force += gravity;
            // apply_force(phys, gravity);
        }
        else
        {
            elastic_collision_immovable(phys);
            // phys.force -= gravity;
            // phys.force = glm::vec3(0,0,0);
        }
        time_step(phys, dt);

        trans.modelToWorld[3] = glm::vec4(phys.pos, 1.0);
    }
}

void PhysicsSystem::elastic_collision_immovable(Phys& p1)
{
    const float mb = 10000000.0f;
    const glm::vec3 vbi = glm::vec3(0.0, 0.0, 0.0);
    Phys p2;
    p2.mass = mb;
    p2.vel = vbi;
    elastic_collision(p1, p2);
}

void PhysicsSystem::elastic_collision(Phys& p1, Phys& p2)
{
    const float ma = p1.mass;
    const float mb = p2.mass;
    const glm::vec3 vai = p1.vel;
    const glm::vec3 vbi = p2.vel;

    p1.vel = (ma - mb)/(ma + mb)*vai + 2 * mb/(ma + mb)*vbi;
    p2.vel = 2*ma / (ma + mb)*vai - (ma - mb)/(ma + mb)*vbi;

    float abs_tol = 1e-6f;
    float rel_tol = 1e-9f;

    float momentum_diff = glm::length(ma*vai + mb*vbi - ma*p1.vel - mb*p2.vel);
    float energy_diff = ma * glm::dot(vai, vai) + mb * glm::dot(vbi, vbi)  - ma * glm::dot(p1.vel, p1.vel) - mb * glm::dot(p2.vel, p2.vel);

    bool preservation_momentum = isclose(momentum_diff, 0.f, rel_tol, abs_tol);
    bool preservation_energy = isclose(energy_diff, 0.f, rel_tol, abs_tol);
    
    assert(preservation_momentum && "momentum was not perserved!");
    assert(preservation_energy && "Energy was not perserved!");
}

void PhysicsSystem::time_step(Phys& phys, float dt){
    phys.acc = phys.force / phys.mass;
    phys.vel += phys.acc * dt;
    phys.pos += phys.vel * dt;
}

ECS::ECS()
{
    if(++_instance_count > _max_instances)
    {
        assert(false && "ECS has been instantiated more than once");
    }
    _entities = std::vector<EntityID>();
    _storages = std::vector<IComponentStore*>();
}

void ECS::addTag(EntityID entity, std::string tag)
{
    _tags[tag] = entity;
}

EntityID ECS::getEntityWithTag(std::string tag)
{
    assert(_tags.count(tag) > 0 && "Tag did not exist");

    return _tags[tag];
}

EntityID ECS::createEntity()
{
    static EntityID id = 0;
    id += 1;
    _entities.push_back(id);
    return id;
}

void ECS::removeEntity(EntityID entity)
{
    for (IComponentStore* storage : _storages)
    {
        storage->removeEntity(entity);
    }
}

