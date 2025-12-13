#include "World.hpp"

World::World()
{
    _entities = std::vector<Entity>();
}

void World::create_world()
{
    for(int x = 0; x < 100; x ++)
    {
        for(int z = 0; z < 100; z ++)
        {
            float y = 0;
            Entity entity = Entity(CUBE, STANDARD);
            entity.setPosition(glm::vec3(x,y,z));
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
