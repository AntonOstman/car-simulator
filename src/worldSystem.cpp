#include "EntityComponentSystem.hpp"
#include "Debug.hpp"
#include <worldSystem.hpp>
#include "Perlin.hpp"
#include "Camera.hpp"

void WorldSystem::create_terrain(ECS &ecs, CompId<Mesh> cubeMesh, CompId<ShaderComp> shader)
{
    int size = 100;
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            const float frequency = 10.f;
            const float amplitude = 4.f;
            const float height = PerlinNoise::perlin_noise_at(1.f/frequency * x, 1.f/frequency * y,0) * amplitude;

            EntitySpawner::create_cube(ecs, x, height, y, cubeMesh, shader);
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

void EntitySpawner::create_cube(ECS &ecs, float x, float y, float z, CompId<Mesh> cubeMesh, CompId<ShaderComp> shader)
{
    Transform cubeMTW;
    cubeMTW.init(0.5f);
    cubeMTW.setTranslation(glm::vec3(x,y,z));

    CompId<Transform> cubeTransId = ecs.createComponent(cubeMTW);
    EntityID cubeid = ecs.createEntity();

    ecs.addComponent(cubeid, cubeTransId);
    ecs.addComponent(cubeid, cubeMesh);
    ecs.addComponent(cubeid, shader);
}
