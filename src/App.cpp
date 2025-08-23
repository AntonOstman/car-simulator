#include "App.hpp"
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <algorithm>
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

#include <chrono>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Math.hpp"
#include "EntityComponentSystem.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

glm::mat4 cuberot;

App::App(int window_width, int window_height)
{
    size_callback(window_width, window_height);
}

void App::createEntities()
{

    // float floor[] = { // NOLINT
    //      1.0f, 0.0f,  -1.0f,
    //      -1.0f, 0.0f, -1.0f,
    //     -1.0f, 0.0f, 1.0f,  
    //
    //      1.0f, 0.0f,  -1.0f,
    //      -1.0f, 0.0f,  1.0f,
    //      1.0f, 0.0f,  1.0f, 
    // };
    //
    // std::vector<Vertex> floorverts;
    //
    // for (int i = 0; i < 6; i++)
    // {
    //     glm::vec2 uv = glm::vec2(0,0);
    //     glm::vec3 norm = glm::vec3(0,1,0);
    //     glm::vec3 pos = glm::vec3(floor[i*3],floor[i*3 + 1],floor[i*3 + 2]);
    //
    //     Vertex floorvert;
    //     floorvert.Pos = pos;
    //     floorvert.TexCoord = uv;
    //     floorvert.Normal = norm;
    //
    //     floorverts.push_back(floorvert);
    // }

    std::vector<Vertex> cubeverts = parseObj("assets/cube.obj");
    std::vector<Vertex> bunnyverts = parseObj("assets/bunny.obj");

    CompId<ShaderComp> program = _renderingSystem.createShader("src/shaders/shaderVertTexNorm.frag", "src/shaders/shaderVertTexNorm.vert", _ecs);

    EntityID bunnyid = _ecs.createEntity();
    EntityID cubeid = _ecs.createEntity();
    EntityID camid = _ecs.createEntity();

    _ecs.addComponent(bunnyid, program);
    _ecs.addComponent(cubeid, program);

    CompId<Mesh> cubeMesh = _renderingSystem.createMesh(cubeverts, _ecs);
    CompId<Mesh> bunnymesh = _renderingSystem.createMesh(bunnyverts, _ecs);

    _worldSystem.create_terrain(_ecs, cubeMesh, program);

    _ecs.addComponent(cubeid, cubeMesh);
    _ecs.addComponent(bunnyid, bunnymesh);

    Transform cubeMTW;
    cubeMTW.init(1.f);

    CompId<Transform> cubeTransId = _ecs.createComponent(cubeMTW);

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

const int size = 100;
int idx = 0;
float prev_times[size];

void App::renderGame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::PlotLines("FPS", prev_times, IM_ARRAYSIZE(prev_times));
    // ImGui::ShowDemoWindow(); // Show demo window! :)
    ImGui::SliderFloat("Fov", &_fov, 20, 120, "%f");
    ImGui::SliderFloat("Viewdist", &_viewDist, 5, 120, "%f");

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    _renderingSystem.update(_ecs);
    auto render = high_resolution_clock::now();
    _physicsSystem.update(_ecs);
    auto physics = high_resolution_clock::now();
    float render_fps = 1000.f / (duration_cast<milliseconds>(render - start).count());
    float phys_fps = 1000.f / (duration_cast<milliseconds>(physics - render).count());

    idx = (idx + 1) % size;

    prev_times[idx] = render_fps;

    ImGui::Text("Render fps: %f", render_fps);
    ImGui::Text("Physics fps: %f", phys_fps);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool key_debounce(uint idx)
{
    using namespace std::chrono;

    const uint keys = GLFW_KEY_LAST;
    static uint times[keys + 1] = {0};
    assert(idx < keys && "idx too large for key debouncing array");

    const uint bounce_duration_ms = 400;
    uint now_time = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();;

    uint prev_time = times[idx];
    times[idx] = now_time;

    uint time_since_last_press = now_time - prev_time;
    std::cout << time_since_last_press << std::endl;

    if(time_since_last_press > bounce_duration_ms)
    {
        return true;
    }

    return false;

}

void App::key_callback(int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

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
    if (key == GLFW_KEY_ESCAPE)
    {
        std::cout << "ecs press" << std::endl;
        if (key_debounce(key))
        {
            _mouseDisabled = !_mouseDisabled;
        }
    }
}

void App::mouse_button_callback(int /*button*/, int /*action*/, int /*mods*/)
{
    WorldSystem::remove_block(_ecs);
}

void App::scroll_callback(double /*xoffset*/, double /*yoffset*/)
{
}


void App::cursor_position_callback(double xpos, double ypos)
{

    EntityID cam = _ecs.getEntityWithTag("mainCamera");
    CameraComp& camComp = _ecs.getComponent<CameraComp>(cam);
    CameraSystem::setPerspective(camComp, _fov, _width, _height, 0.1f, _viewDist);

    if (ImGui::GetIO().WantCaptureMouse || _mouseDisabled)
    {
        return;
    }

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
    glViewport(0, 0, width, height);
}
