#include "EntityComponentSystem.hpp"
#include "Debug.hpp"
#include <iostream>
#include <ostream>
#include <worldSystem.hpp>
#include "Perlin.hpp"
#include "Camera.hpp"

std::vector<glm::ivec3> WorldSystem::active_chunks()
{
    std::vector<glm::ivec3> chunks;
    return chunks;
}

glm::ivec3 Chunk::getChunkToWorld(glm::ivec3 pos)
{
    return pos * (int) CHUNK_SIZE;
}

// TODO should not exist, test function for now..
void WorldSystem::create_chunks()
{
    // TODO these should be privates in chunk.......
    for (int i = 0; i < 2; i ++)
    {
        for (int j = 0; j < 2; j ++)
        {
            Chunk chunk = Chunk();
            chunk.position = glm::ivec3(64 * i, 0, 64 * j);
            _chunks.push_back(chunk);
        }
    }
}

// TODO can probably be optimised with hash maps... fine for now
void WorldSystem::create_terrain(ECS &ecs)
{
    for (Chunk chunk : _chunks)
    {
        bool exists = false;
        for (auto chunkpos : active_chunks())
        {
            if (chunkpos == chunk.position)
            {
                exists = true;
            }
        }

        if (!exists)
        {
            std::cout << "creating chunk" << std::endl;
            chunk.createChunk(ecs);
            _chunks.push_back(chunk);
        }
        else
        {
            // std::cout << "removing chunk" << std::endl;
            // chunk.removeChunk(ecs);
        }
    }
}

void WorldSystem::remove_block(ECS &ecs)
{
    EntityID entity = player_raycast(ecs);

    if (entity != INVALID_ENTITY)
    {
        ecs.removeEntity(entity);
    }
}


void WorldSystem::place_block(ECS &ecs)
{
    EntityID entity = player_raycast(ecs);

    if (entity != INVALID_ENTITY)
    {
        Transform trans = ecs.getComponent<Transform>(entity);
        glm::vec3 newpos = trans.getTranslation() + glm::vec3(0,trans.getSize().y,0);
        EntitySpawner::create_cube(ecs, newpos, "block", "standard");
    }
}

EntityID WorldSystem::player_raycast(ECS &ecs)
{
    std::vector<EntityID> entities = ecs.intersection_entity_id<Transform, Mesh>();
    EntityID mainCameraID = ecs.getEntityWithTag("mainCamera");

    CameraComp mainCamera = ecs.getComponent<CameraComp>(mainCameraID);

    const float maxRay = 5.f;
    const float rayStep = 0.3f;

    glm::vec3 rayDir = CameraSystem::getForwardWorld(mainCamera);
    glm::vec3 cameraPos = CameraSystem::getWorldPos(mainCamera);

    // TODO optimize raycasting, do we really need to check all entites
    // idea kdtree
    for (float raylen = 0; raylen < maxRay; raylen += rayStep)
    {
        glm::vec3 rayPos = raylen * rayDir + cameraPos;
        for (uint i = 0; i < entities.size(); i++)
        {
            EntityID entity = entities[i];
            Transform& trans = ecs.getComponent<Transform>(entity);
            AABB box = trans.getAABB();

            if (box.isInside(rayPos))
            {
                return entity;
            }
        }
    }
    return INVALID_ENTITY;
}

EntityID EntitySpawner::create_cube(ECS& ecs, float x, float y, float z, std::string /*mesh_name*/, std::string /*shader_name*/)
{
    Transform cubeMTW;
    cubeMTW.init(0.5f);
    cubeMTW.setTranslation(glm::vec3(x,y,z));

    CompId<Transform> cubeTransId = ecs.createComponent(cubeMTW);
    EntityID cubeid = ecs.createEntity();

    ecs.addComponent(cubeid, cubeTransId);
    ecs.addComponent(cubeid, ecs.getNamedComponent<Mesh>("block"));
    ecs.addComponent(cubeid, ecs.getNamedComponent<ShaderComp>("standard"));
    return cubeid;
}

void Chunk::handleEdit(ECS& ecs, glm::uvec3& pos, Edit edit)
{
    switch(edit.edit_type)
    {
        case editType::REMOVE:
            {
                break;
            }
        case editType::ADD:
            {
                EntityID entity = EntitySpawner::create_cube(ecs, pos, "block","standard");
                _entities.push_back(entity);
                break;
            }
        default:
            break;
    }
}

void Chunk::createChunk(ECS& ecs)
{
    for (uint x = 0; x < CHUNK_SIZE; x++)
    {
        for (uint z = 0; z < CHUNK_SIZE; z++)
        {
            const float frequency = 10.f;
            const float amplitude = 4.f;
            const float dc = 0.f;
            float noisex = 1.f/frequency * (float)(x + position.x);
            float noisez = 1.f/frequency * (float)(z + position.z);

            const float height = PerlinNoise::perlin_noise_at(noisex, noisez, 0) * amplitude + dc;

            for (uint y = 0; y < CHUNK_SIZE; y++)
            {
                if (y > height)
                {
                    break;
                }

                glm::uvec3 pos = glm::uvec3(x,y,z);
                if (_edits.count(uvec3hash(pos)) > 0)
                {
                   Edit edit = _edits[uvec3hash(pos)];
                   handleEdit(ecs, pos, edit);
                }

                EntitySpawner::create_cube(ecs, pos, "block", "standard");
                glm::vec3 worldpos = glm::vec3(position.x + x, height, position.z + z);

                EntityID entity = EntitySpawner::create_cube(ecs, worldpos, "block", "standard");
                _entities.push_back(entity);
                break;

            }
        }
    }
}

void Chunk::removeChunk(ECS& ecs)
{
    for (uint i = 0; i < _entities.size(); i++)
    {
        ecs.removeEntity(_entities[i]);
    }
    _entities.clear();
}

Vec3Hash Chunk::uvec3hash(glm::uvec3& pos)
{
    uint32_t x = pos.x;
    uint32_t y = pos.y;
    uint32_t z = pos.z;

    uint32_t max = glm::max(glm::max(x,y), z);
    Vec3Hash hash = (max >> 3) + (2 * max * z ) + z;
    if(max == z)
    {
        hash += (glm::max(x,y) >> 2);
    }
    if(y >= x)
    {
        hash += x + y;
    }

    return hash;
}
