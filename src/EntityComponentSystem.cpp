#include "EntityComponentSystem.hpp"
#include "Model.hpp"
#include "glad/glad.h"
#include "Debug.hpp"
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <unordered_map>
#include <cassert>
#include <vector>

std::string read_file(std::string &filename){

    if (filename == "")
    {
        std::cout << "ERROR::App::read_file No filename specified" << std::endl;
        exit(0);
    }
    
    std::ifstream file;
    file.open(filename);
    std::string buffer;
    std::string line;

    while(file){
        std::getline(file, line);
        line.append("\n");
        buffer.append(line);
    }
    file.close();

    return buffer;
}

unsigned int compile_shader(std::string vertex_name, std::string fragment_name){
    
    std::string frag = read_file(fragment_name);
    std::string vert = read_file(vertex_name);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vert_c_str = vert.c_str();
    glShaderSource(vertexShader, 1, &vert_c_str, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* frag_c_str = frag.c_str();
    glShaderSource(fragmentShader, 1, &frag_c_str, nullptr);
    glCompileShader(fragmentShader);

    int  success = 0;
    char infoLog[512]; // NOLINT

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << vertex_name << std::endl << infoLog << std::endl;
        exit(1);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << fragment_name << std::endl << infoLog << std::endl;
        exit(1);
    }

    unsigned int shaderProgram = 0;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::LINKING::COMPILATION_FAILED\n" << fragment_name << " " << vertex_name << "\n" << infoLog << std::endl;
        exit(1);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    std::cout << "Compile shader " << vertex_name << " " << fragment_name << " success" << std::endl;

    return shaderProgram;
}
void PhysicsSystem::time_step(Phys phys, float dt){
    phys.acc = phys.force / phys.mass;
    phys.vel += phys.acc * dt;
    phys.pos += phys.vel * dt;
}

void PhysicsSystem::apply_force(Phys phys, glm::vec3 force){
    phys.force += force; 
}

void RenderingSystem::setUniforms(const GLuint& program, const glm::mat4& modelp, const glm::mat4& view, const glm::mat4& projection)
{
    glUseProgram(program);

    GLuint loc;
    printError("before RenderingSystem::setUniforms");

    loc = glGetUniformLocation(program, "model");
    printError("model loc RenderingSystem::setUniforms");
    glUniformMatrix4fv(loc, 1, false, &modelp[0][0]);
    printError("model RenderingSystem::setUniforms");

    loc = glGetUniformLocation(program, "view");
    printError("view loc RenderingSystem::setUniforms");
    glUniformMatrix4fv(loc, 1, false, &view[0][0]);
    printError("view RenderingSystem::setUniforms");

    loc = glGetUniformLocation(program, "projection");
    printError("projection loc RenderingSystem::setUniforms");
    glUniformMatrix4fv(loc, 1, false, &projection[0][0]);
    printError("projection RenderingSystem::setUniforms");
} 

void RenderingSystem::update(ECS& ecs)
{

    printError("before RenderingSystem::update");
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printError("after RenderingSystem::update");

    EntityID cameraEntity = ecs.getEntityWithTag("mainCamera");
    CameraComp mainCamera = ecs.getComponent<CameraComp>(cameraEntity);
    glm::mat4 view = mainCamera.view;
    glm::mat4 perspective = mainCamera.perspective;

    std::vector<EntityID> drawables = ecs.intersection_entity_id<Transform, Mesh, ShaderComp>();

    if (drawables.empty())
    {
        std::cout << "No drawables found" << std::endl;
    }

    for (EntityID drawable : drawables)
    {
        Transform transform = ecs.getComponent<Transform>(drawable);
        Mesh mesh = ecs.getComponent<Mesh>(drawable);
        ShaderComp shader = ecs.getComponent<ShaderComp>(drawable);
        glm::mat4 model = transform.modelToWorld;

        RenderingSystem::setUniforms(shader.program, model, view, perspective);
        RenderingSystem::drawTriangles(mesh, shader.program);
    }
}


ECS::ECS()
{
    _entities = std::vector<EntityID>();
    _storages = std::vector<IComponentStore*>();
}

// template<typename T, typename U>
// std::unordered_set<EntityID> ECS::intersection_entity_id()
// {
//     std::unordered_set<EntityID> intersections = std::unordered_set<EntityID>();
//
//     for (uint i = 0; i < _entities.size(); i++)
//     {
//         EntityID entity = _entities[i];
//         if (getStorage<T>().entities.count(entity) && getStorage<U>().entities.count(entity))
//         {
//             intersections.insert(entity);
//         }
//     }
//     return intersections;
// }

template<typename... Components>
std::vector<EntityID> ECS::intersection_entity_id() {
    std::vector<EntityID> intersections;

    for (EntityID entity : _entities) {
        if ((getStorage<Components>().entities.count(entity) && ...)) {
            intersections.push_back(entity);
        }
    }
    return intersections;
}

// template<typename T>
// std::unordered_set<EntityID> ECS::intersection_entity_id(std::unordered_set<EntityID> entities)
// {
//     std::unordered_set<EntityID> intersections = std::unordered_set<EntityID>();
//
//     for (EntityID entity : entities)
//     {
//         if (getStorage<T>().entities.count(entity))
//         {
//             intersections.insert(entity);
//         }
//     }
//     return intersections;
//
// }

// void ECS::updateEntities()
// {
//     EntityID mainCamera = getEntityWithTag("mainCamera");
//     glm::mat4 view = _cameras[mainCamera]->view;
//     glm::mat4 perspective = _cameras[mainCamera]->perspective;
//
//     std::vector<EntityID> drawables = intersection_entity_id(_transforms, _meshes);
//     for (EntityID drawable : drawables)
//     {
//         glm::mat4 model = _transforms[drawable]->modelToWorld;
//         GLuint program = _meshes[drawable]->program;
//         RenderingSystem::setUniforms(program, model, view, perspective);
//         RenderingSystem::drawTriangles(*_meshes[drawable], program);
//     }
// }

void ECS::addTag(EntityID entity, std::string tag)
{
    _tags[tag] = entity;
}

EntityID ECS::getEntityWithTag(std::string tag)
{
    assert(_tags.count(tag) > 0 && "Tag did not exist");

    return _tags[tag];
}

CompId<ShaderComp> RenderingSystem::createShader(std::string frag, std::string vert, ECS& ecs)
{
    ShaderComp shader;
    GLuint program = compile_shader(vert, frag);
    shader.program = program;
    return ecs.createComponent(shader);
}
        
CompId<Mesh> RenderingSystem::createMesh(const std::vector<Vertex> &vertices, ECS& ecs)
{
    Mesh mesh;
    GLuint VAO = 0;
    GLuint VBO = 0;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    mesh.VAO = VAO;
    mesh.VBO = VBO;
    mesh.num_vert = vertices.size();
    printError("Model::generateBuffers");
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Pos)));
    printError("Model::bind::vertex");

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoord)));
    printError("Model::bind::tex");

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
    printError("Model::bind::normal");

    glBindVertexArray(0);
    return ecs.createComponent(mesh);
}

void RenderingSystem::drawTriangles(Mesh& mesh, GLuint program)
{
    printError("before RenderingSystem::drawTriangles");
    assert(mesh.VAO != 0 && "Uninitialized VAO");
    assert(program != 0 && "Uninitialized program");
    assert(mesh.num_vert != 0 && "No vertices in mesh");

    glBindVertexArray(mesh.VAO);
    glUseProgram(program);

    printError("RenderingSystem::drawTriangles bind error");
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, mesh.num_vert);
    printError("RenderingSystem::drawTriangles draw error");

    glBindVertexArray(0);
}

void RenderingSystem::drawLines(Mesh& mesh, GLuint program)
{
    printError("before RenderingSystem::drawLines");
    glBindVertexArray(mesh.VAO);
    glUseProgram(program);

    printError("RenderingSystem::drawLines bind error");
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINES, 0, mesh.num_vert);
    printError("RenderingSystem::drawLines draw error");

    glBindVertexArray(0);
}


EntityID ECS::createEntity()
{
    static EntityID id = 0;
    id += 1;
    _entities.push_back(id);
    return id;
}

void ECS::removeEntity(EntityID entity)
{
    for (IComponentStore* storage : _storages)
    {
        storage->removeEntity(entity);
    }
}

template<typename T>
ComponentStore<T>& ECS::getStorage()
{
    // Sanity check each component to the extreme
    static_assert(!std::is_pointer_v<T>, "Component type cannot be a pointer.");
    static_assert(!std::is_reference_v<T>, "Component type cannot be a reference.");
    static_assert(!std::is_const_v<T>, "Component type cannot be const.");
    static_assert(!std::is_volatile_v<T>, "Component type cannot be volatile.");
    static_assert(sizeof(T) > 0, "Component type must be fully defined before use.");
    static_assert(std::is_trivially_copyable_v<T>,
                  "Component type must be trivially copyable for this ECS.");
    static_assert(!std::is_base_of_v<IComponentStore, T>,
                  "You passed a storage type instead of a component type.");

    // Make sure each component is marked as a component
    static_assert(std::is_base_of_v<ComponentMarker, T>,
                  "Component type must inherit ComponentMarker");


    static ComponentStore<T> storage = ComponentStore<T>{};
    static bool registered = false;
    if (!registered)
    {
        _storages.push_back(&storage);
        registered = true;
    }
    return storage;
}
