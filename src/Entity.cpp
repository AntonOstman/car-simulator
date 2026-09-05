#include "RenderingSystem.hpp"
#include "Entity.hpp"
#include <iostream>

// float dt = 1.f;

Entity::Entity(MeshType mesh, ShaderType shader)
{
    _mesh = mesh;
    _shader = shader;
    _phys = Phys();
    Transform trans;
    trans.init(1.0);
    _transform = trans;
    _size = 1;
}

Entity::Entity()
{
   Entity(MeshType::NONE, ShaderType::NONE);
}

void Entity::render(glm::mat4 worldToView, glm::mat4 projection)
{
    RenderingSystem::renderMesh(_mesh, _shader, _transform.modelToWorld, worldToView, projection);
}

void Entity::setPosition(glm::vec3 pos)
{
    _transform.setTranslation(pos);
}

void Entity::move(glm::vec3 velocity)
{
   glm::vec3 rotated_vel = _camera.getRotation() * velocity;
   _phys.vel += rotated_vel;
}

void Entity::moveSimple(glm::vec3 velocity)
{
   glm::vec3 rotated_vel = _camera.getRotation() * velocity;
   _phys.vel = rotated_vel;
}

void Entity::addAcceleration(glm::vec3 acc)
{
   _phys.acc += acc;
}

static void collision(Phys &p1, Phys &p2)
{
   p1.vel = glm::vec3(0);
   p2.vel = glm::vec3(0);
}

void Entity::physics_update(float dt)
{
    float gravity = 9.82;
    if (_phys.pos.y < 0)
    {
       gravity = 0;
       collision(_phys,_phys);
       _phys.vel.y = 0.0;
    }
    _phys.acc.y -= gravity;

   _phys.force = _phys.acc * _phys.mass;
   _phys.vel += _phys.acc * dt;
   _phys.pos += _phys.vel * dt;
   std::cout << _phys.pos.x << std::endl;
}

void Entity::transform_update()
{
   _transform.setTranslation(_phys.pos);
   _camera.setTranslationWorld(_phys.pos);
}
