#pragma once

#include "EntityComponentSystem.hpp"

class EntitySpawner
{
public:
    EntityID static create_cube(ECS& ecs, float x, float y, float z, std::string mesh_name, std::string shader_name);

    EntityID static create_cube(ECS &ecs, glm::vec3 pos, std::string cubeMesh, std::string shader)
    {
        return create_cube(ecs, pos.x, pos.y, pos.z, cubeMesh, shader);
    }
};


enum blockType{
    DIRT
};

enum editType{
        REMOVE,
        ADD
};

struct Edit
{
    blockType block_type;
    editType edit_type;
};

using Vec3Hash = uint32_t;

class Chunk
{
    const static uint CHUNK_SIZE = 64;

public:
    glm::ivec3 position;
private:
    std::vector<EntityID> _entities;
    std::unordered_map<Vec3Hash, Edit> _edits;

public:
    void createChunk(ECS& ecs);
    void removeChunk(ECS& ecs);
    glm::ivec3 static getChunkToWorld(glm::ivec3 pos);
private:
    void handleEdit(ECS& ecs, glm::uvec3& pos, Edit edit);
    Vec3Hash uvec3hash(glm::uvec3& pos);

};


class WorldSystem
{
private:
    std::vector<Chunk> _chunks;

public:
    void create_chunks();
    void create_terrain(ECS &ecs);
    static EntityID player_raycast(ECS &ecs);
    static void remove_block(ECS &ecs);
    void static place_block(ECS &ecs);
    std::vector<glm::ivec3> static active_chunks();

};
