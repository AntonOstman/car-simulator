#include "RenderingSystem.hpp"
#include "Entity.hpp"

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

void Entity::render(glm::mat4 worldToView, glm::mat4 projection)
{
    RenderingSystem::renderMesh(_mesh, _shader, _transform.modelToWorld, worldToView, projection);
}

void Entity::setPosition(glm::vec3 pos)
{
    _transform.setTranslation(pos);
}
