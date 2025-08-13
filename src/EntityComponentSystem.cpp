#include "EntityComponentSystem.hpp"
#include "glad/glad.h"
#include "Debug.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>
#include <math.h>
#include "Math.hpp"

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

Phys PhysicsSystem::createPhysComp()
{
    Phys comp;
    comp.force = glm::vec3(0,0,0);
    comp.acc = glm::vec3(0,0,0);
    comp.pos = glm::vec3(0,10,0);
    comp.vel = glm::vec3(0,0,0);
    comp.mass = 1.0;
    return comp;
}

void PhysicsSystem::update(ECS ecs){

    std::vector entities = ecs.intersection_entity_id<Transform, Mesh, Phys>();
    float dt = 0.1;

    glm::vec3 gravity = glm::vec3(0,-0.01,0);

    for (EntityID entity : entities)
    {
        Phys& phys = ecs.getComponent<Phys>(entity);
        phys.force = glm::vec3(0);
        Transform& trans = ecs.getComponent<Transform>(entity);
        if (phys.pos.y > 0)
        {
            phys.force += gravity;
            // apply_force(phys, gravity);
        }
        else
        {
            elastic_collision_immovable(phys);
            // phys.force -= gravity;
            // phys.force = glm::vec3(0,0,0);
        }
        time_step(phys, dt);

        trans.modelToWorld[3] = glm::vec4(phys.pos, 1.0);
    }
}

void PhysicsSystem::elastic_collision_immovable(Phys& p1)
{
    const float mb = 10000000.0f;
    const glm::vec3 vbi = glm::vec3(0.0, 0.0, 0.0);
    Phys p2;
    p2.mass = mb;
    p2.vel = vbi;
    elastic_collision(p1, p2);
}

void PhysicsSystem::elastic_collision(Phys& p1, Phys& p2)
{

    const float ma = p1.mass;
    const float mb = p2.mass;
    const glm::vec3 vai = p1.vel;
    const glm::vec3 vbi = p2.vel;

    p1.vel = (ma - mb)/(ma + mb)*vai + 2 * mb/(ma + mb)*vbi;
    p2.vel = 2*ma / (ma + mb)*vai - (ma - mb)/(ma + mb)*vbi;

    float abs_tol = 1e-6f;
    float rel_tol = 1e-9f;

    float momentum_diff = glm::length(ma*vai + mb*vbi - ma*p1.vel - mb*p2.vel);
    float energy_diff = ma * glm::dot(vai, vai) + mb * glm::dot(vbi, vbi)  - ma * glm::dot(p1.vel, p1.vel) - mb * glm::dot(p2.vel, p2.vel);

    bool preservation_momentum = isclose(momentum_diff, 0.f, rel_tol, abs_tol);
    bool preservation_energy = isclose(energy_diff, 0.f, rel_tol, abs_tol);
    
    assert(preservation_momentum && "momentum was not perserved!");
    assert(preservation_energy && "Energy was not perserved!");
}

void PhysicsSystem::time_step(Phys& phys, float dt){
    phys.acc = phys.force / phys.mass;
    phys.vel += phys.acc * dt;
    phys.pos += phys.vel * dt;
}

void RenderingSystem::setUniforms(const GLuint& program, const glm::mat4& modelp, const glm::mat4& view, const glm::mat4& projection)
{
    glUseProgram(program);

    printError("before RenderingSystem::setUniforms");

    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, false, &modelp[0][0]);
    printError("model RenderingSystem::setUniforms");

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, false, &view[0][0]);
    printError("view RenderingSystem::setUniforms");

    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, false, &projection[0][0]);
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
    glm::mat4 view = glm::inverse(mainCamera.view);
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
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
    // Sanity check each type before creation.
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
