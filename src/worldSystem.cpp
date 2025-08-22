#include "EntityComponentSystem.hpp"
#include "Debug.hpp"
#include <worldSystem.hpp>
#include "Perlin.hpp"

void WorldSystem::create_terrain(ECS &ecs, CompId<Mesh> cubeMesh, CompId<ShaderComp> shader)
{
    int size = 100;
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            float sample_time = 0.1;
            float amplitude = 4.f;
            float z = PerlinNoise::perlin_noise_at(sample_time * x, sample_time * y,0) * amplitude;

            EntitySpawner::create_cube(ecs, x, y,z, cubeMesh, shader);
        }
    }
}

void EntitySpawner::create_cube(ECS &ecs, float x, float y, float z, CompId<Mesh> cubeMesh, CompId<ShaderComp> shader)
{
    Transform cubeMTW;
    cubeMTW.init(0.45);
    cubeMTW.setTranslation(glm::vec3(x,z,y));

    CompId<Transform> cubeTransId = ecs.createComponent(cubeMTW);
    EntityID cubeid = ecs.createEntity();

    ecs.addComponent(cubeid, cubeTransId);
    ecs.addComponent(cubeid, cubeMesh);
    ecs.addComponent(cubeid, shader);
}
