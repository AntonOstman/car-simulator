#pragma once
#include <vector>
#include "Entity.hpp"

class World
{
    public:
        World();
        void create_world();
        void renderWorld(glm::mat4 worldToView, glm::mat4 projection);
    private:
        std::vector<Entity> _entities;
};
