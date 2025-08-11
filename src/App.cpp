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
#include "Model.hpp"
#include "Camera.hpp"
#include "glm/matrix.hpp"
#include "Loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Math.hpp"
#include "EntityComponentSystem.hpp"

glm::mat4 cuberot;
Camera camera;
Camera cameraStill;
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
    CompId<Mesh> cubemesh = _renderingSystem.createMesh(cubeverts, _ecs);
    CompId<Mesh> bunnymesh = _renderingSystem.createMesh(bunnyverts, _ecs);

    _ecs.addComponent(floorid, floormesh);
    _ecs.addComponent(cubeid, cubemesh);
    _ecs.addComponent(bunnyid, bunnymesh);

    glm::mat4 floorTrans = scaled_eye(1000.0);
    glm::mat4 cubeTrans = scaled_eye(1.0);
    Transform floorMTW;
    Transform cubeMTW;
    floorMTW.modelToWorld = floorTrans;
    cubeMTW.modelToWorld = cubeTrans;

    CompId<Transform> floorTransId = _ecs.createComponent(floorMTW);
    CompId<Transform> cubeTransId = _ecs.createComponent(cubeMTW);

    _ecs.addComponent(floorid, floorTransId);
    _ecs.addComponent(cubeid, cubeTransId);

    glm::mat4 perspective = glm::perspectiveFov(glm::radians(45.0f), (float) _width, (float) _height, 0.1f, 20.0f);
    camera.setTranslationWorld(glm::vec3(0,1,10));
    CameraComp camcomp;
    camcomp.view = glm::inverse(camera.getViewToWorld());
    camcomp.perspective = perspective;
    CompId<CameraComp> camcompid = _ecs.createComponent(camcomp);
    _ecs.addComponent(camid, camcompid);
    _ecs.addTag(camid, "mainCamera");

    // cameraStill.setTranslationWorld(glm::vec3(0,2,10));
}

void App::init()
{
    // glDepthMask(GL_TRUE);
    // glViewport(0, 0, _width, _height); // Set viewport
    _ecs = ECS();
    createEntities();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("assets/cube.obj", aiProcess_Triangulate);
    scene->HasCameras();
}

void App::updateSystems()
{
}
void App::render()
{
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // double timeValue = glfwGetTime();
    // float greenValue = sin(timeValue) / 2.0f + 0.5f;
    // float angle = greenValue;
    // cuberot = Ry(glm::degrees(angle));
    //
    // glm::mat4 worldToView;
    // if (cameraStillChosen)
    // {
    //     worldToView = glm::inverse(cameraStill.getViewToWorld());
    // }
    // else
    // {
    //     worldToView = glm::inverse(camera.getViewToWorld());
    // }
    //
    // // glm::mat4 perspective = glm::perspectiveFov(glm::radians(45.0f), (float) _width, (float) _height, 0.1f, 20.0f);
    // glm::mat4 modelToWorld = scaled_eye(1000.0);
    //
    // modelToWorld = cuberot * scaled_eye(1.0);
    // _ecs.updateEntities();
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // updateSystems();
    _renderingSystem.update(_ecs);
}

void App::key_callback(int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
    float speed = 0.1;
    if (key == GLFW_KEY_W)
    {
        camera.moveForward(speed);
    }
    if (key == GLFW_KEY_S)
    {
        camera.moveBack(speed);
    }

    if (key == GLFW_KEY_A)
    {
        camera.moveLeft(speed);
    }

    if (key == GLFW_KEY_D)
    {
        camera.moveRight(speed);
    }

    if (key == GLFW_KEY_Y)
    {
        camera.moveUp(speed);
    }
    if (key == GLFW_KEY_U)
    {
        camera.moveDown(speed);
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

    camera.rotateRelative(glm::vec2(dx, dy));
    prev_xpos = xpos;
    prev_ypos = ypos;
}

void App::size_callback(int width, int height)
{
    _width  = width;
    _height = height;
    // glViewport(0, 0, width, height); // Set viewport explicitly
}
