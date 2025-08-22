#pragma once

#include "EntityComponentSystem.hpp"
class WorldSystem
{
    public:
    static void create_terrain(ECS &ecs, CompId<Mesh> cubeMesh, CompId<ShaderComp> shader);
};


class EntitySpawner
{
    public:
    static void create_cube(ECS &ecs, float x, float y, float z, CompId<Mesh> cubeMesh, CompId<ShaderComp> shaderComp);
};
