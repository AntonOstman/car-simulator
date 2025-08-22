#include "App.hpp"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <fstream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include "Camera.hpp"
#include "glm/matrix.hpp"
#include "Loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Math.hpp"
#include "EntityComponentSystem.hpp"

glm::mat4 cuberot;
bool cameraStillChosen = false;

App::App(int window_width, int window_height)
{
    size_callback(window_width, window_height);
}

void App::createEntities()
{

    float floor[] = { // NOLINT
         1.0f, 0.0f,  -1.0f,
         -1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 1.0f,  

         1.0f, 0.0f,  -1.0f,
         -1.0f, 0.0f,  1.0f,
         1.0f, 0.0f,  1.0f, 
    };

    std::vector<Vertex> floorverts;

    for (int i = 0; i < 6; i++)
    {
        glm::vec2 uv = glm::vec2(0,0);
        glm::vec3 norm = glm::vec3(0,1,0);
        glm::vec3 pos = glm::vec3(floor[i*3],floor[i*3 + 1],floor[i*3 + 2]);

        Vertex floorvert;
        floorvert.Pos = pos;
        floorvert.TexCoord = uv;
        floorvert.Normal = norm;

        floorverts.push_back(floorvert);
    }
    std::vector<Vertex> cubeverts = parseObj("assets/cube.obj");
    std::vector<Vertex> bunnyverts = parseObj("assets/bunny.obj");

    CompId<ShaderComp> program = _renderingSystem.createShader("src/shaders/shaderVertTexNorm.frag", "src/shaders/shaderVertTexNorm.vert", _ecs);

    EntityID floorid = _ecs.createEntity();
    EntityID bunnyid = _ecs.createEntity();
    EntityID cubeid = _ecs.createEntity();
    EntityID camid = _ecs.createEntity();

    _ecs.addComponent(floorid, program);
    _ecs.addComponent(bunnyid, program);
    _ecs.addComponent(cubeid, program);

    CompId<Mesh> floormesh = _renderingSystem.createMesh(floorverts, _ecs);
    CompId<Mesh> cubeMesh = _renderingSystem.createMesh(cubeverts, _ecs);
    CompId<Mesh> bunnymesh = _renderingSystem.createMesh(bunnyverts, _ecs);

    _worldSystem.create_terrain(_ecs, cubeMesh, program);

    _ecs.addComponent(floorid, floormesh);
    _ecs.addComponent(cubeid, cubeMesh);
    _ecs.addComponent(bunnyid, bunnymesh);

    Transform floorMTW;
    Transform cubeMTW;
    floorMTW.init(1000.f);
    cubeMTW.init(1.f);

    CompId<Transform> floorTransId = _ecs.createComponent(floorMTW);
    CompId<Transform> cubeTransId = _ecs.createComponent(cubeMTW);

    _ecs.addComponent(floorid, floorTransId);
    _ecs.addComponent(cubeid, cubeTransId);

    Phys cubePhys = _physicsSystem.createPhysComp();
    CompId<Phys> cube_phys_id = _ecs.createComponent(cubePhys);
    _ecs.addComponent(cubeid, cube_phys_id);

    glm::mat4 perspective = glm::perspectiveFov(glm::radians(45.0f), (float) _width, (float) _height, 0.1f, 30.0f);
    CameraComp camcomp;
    CameraSystem::init(camcomp);
    CameraSystem::setTranslationWorld(camcomp, glm::vec3(0,1,10));

    camcomp.perspective = perspective;

    CompId<CameraComp> camcompid = _ecs.createComponent(camcomp);
    _ecs.addComponent(camid, camcompid);
    _ecs.addTag(camid, "mainCamera");
}

void App::init()
{
    _renderingSystem.init();
    createEntities();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("assets/cube.obj", aiProcess_Triangulate);
    scene->HasCameras();
}

void App::render()
{
    _renderingSystem.update(_ecs);
    _physicsSystem.update(_ecs);
}

void App::key_callback(int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
    float speed = 0.1;
    EntityID cam = _ecs.getEntityWithTag("mainCamera");
    CameraComp& camComp = _ecs.getComponent<CameraComp>(cam);
     
    if (key == GLFW_KEY_W)
    {
        CameraSystem::moveForward(camComp, speed);
    }
    if (key == GLFW_KEY_S)
    {
        CameraSystem::moveBack(camComp, speed);
    }

    if (key == GLFW_KEY_A)
    {
        CameraSystem::moveLeft(camComp, speed);
    }

    if (key == GLFW_KEY_D)
    {
        CameraSystem::moveRight(camComp, speed);
    }

    if (key == GLFW_KEY_Y)
    {
        CameraSystem::moveUp(camComp, speed);
    }
    if (key == GLFW_KEY_U)
    {
        CameraSystem::moveDown(camComp, speed);
    }

    if (key == GLFW_KEY_C)
    {
        static int counter = 0;
        if (counter < 20)
        {
            counter += 1;
        }
        else
        {
            cameraStillChosen = not cameraStillChosen;
            counter = 0;
        }
    }
}

void App::mouse_button_callback(int /*button*/, int /*action*/, int /*mods*/)
{
}

void App::scroll_callback(double /*xoffset*/, double /*yoffset*/)
{
}


void App::cursor_position_callback(double xpos, double ypos)
{
    EntityID cam = _ecs.getEntityWithTag("mainCamera");
    CameraComp& camComp = _ecs.getComponent<CameraComp>(cam);

    static double prev_xpos = 0;
    static double prev_ypos = 0;
    static bool inited = false;

    if (!inited)
    {
        inited = true;
        prev_xpos = xpos;
        prev_ypos = ypos;
    }

    double dx = (prev_xpos - xpos);
    double dy = (prev_ypos - ypos);
    // std::cout << dx << std::endl;
    // std::cout << dy << std::endl;

    CameraSystem::rotateRelative(camComp, glm::vec2(dx, dy));
    prev_xpos = xpos;
    prev_ypos = ypos;
}

void App::size_callback(int width, int height)
{
    _width  = width;
    _height = height;
    // glViewport(0, 0, width, height); // Set viewport explicitly
}
