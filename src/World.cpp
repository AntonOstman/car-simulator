#include "World.hpp"
#include "Perlin.hpp"
#include <iostream>

World::World()
{
    _entities = std::vector<Entity>();
}

void World::create_world()
{
    for(float x = 0; x < 100; x ++)
    {
        for(float z = 0; z < 100; z ++)
        {
            Entity entity = Entity(MeshType::CUBE, ShaderType::STANDARD);

            float height = PerlinNoise::perlin_noise_at(x/10.f,z/10.f, 42);

            entity.setPosition(glm::vec3(x,height,z));
            _entities.push_back(entity);
        }
    }
}

void World::renderWorld(glm::mat4 worldToView, glm::mat4 projection)
{
    for (Entity entity : _entities)
    {
        entity.render(worldToView, projection);
    }
}

// bool World::rayCastCollision(AABB box, glm::vec3 direction)
// {
//    float step = 0.1f;
//    float length = 4.0f;
//
//    for (int i = 0; i < step; i ++)
//    {
//
//    }


// }
