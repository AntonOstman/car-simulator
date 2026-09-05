#include "App.hpp"
#include "Math.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <math.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate
#include <iostream>
#include "Camera.hpp"
#include "Loader.hpp"
#include "Debug.hpp"

#include <vector>
#include <chrono>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "RenderingSystem.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "UI.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


static Entity player;
static Entity dude(MeshType::CUBE, ShaderType::STANDARD);
static std::vector<Entity> entities;
static bool use_physics = false;

void App::init()
{
    _UIsettings.debugUI = false;
    _UIsettings.drawLines = false;
    player._camera.setView();
    player._camera.setPerspective(45, _width,_height,0.1, 30);
    RenderingSystem::init();
    _world.create_world();
    entities.push_back(dude);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("assets/cube.obj", aiProcess_Triangulate);
    scene->HasCameras();
}

App::App(int window_width, int window_height)
{
    size_callback(window_width, window_height);
}

void App::gameLoop()
{
   gameUpdate();
   renderGame();
}

void App::renderGame()
{
    static const int size = 100;
    static int idx = 0;
    static float prev_times[size];
    printError("before RenderingSystem::update");
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printError("after RenderingSystem::update");
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PlotLines("FPS", prev_times, IM_ARRAYSIZE(prev_times));
    ImGui::SliderFloat("Fov", &_fov, 20, 120, "%f");
    ImGui::SliderFloat("Viewdist", &_viewDist, 5, 300, "%f");
    ImGui::SliderFloat("Cubepos", &_cubePos, -300, 300, "%f");
    ImGui::Checkbox("Draw lines", &_UIsettings.drawLines);
    ImGui::Checkbox("Debug ui", &_UIsettings.debugUI);

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    auto render = high_resolution_clock::now();
    _world.renderWorld(player._camera.getWorldToView(), player._camera.getPerspective());
    float render_fps = 1000.f / (duration_cast<milliseconds>(render - start).count());
    auto physics = high_resolution_clock::now();
    float phys_fps = 1000.f / (duration_cast<milliseconds>(physics - render).count());


    idx = (idx + 1) % size;

    prev_times[idx] = render_fps;

    ImGui::Text("Render fps: %f", render_fps);
    ImGui::Text("Physics fps: %f", phys_fps);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::gameUpdate()
{
   using namespace std::chrono;

   auto start = high_resolution_clock::now();
   static auto prev = high_resolution_clock::now();
   float dt = duration_cast<milliseconds>(start - prev).count() / 1000.f;
   player.physics_update(dt);
   prev = start;
   player.transform_update();
}


bool keyDebounce(uint idx)
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

    return time_since_last_press > bounce_duration_ms;

}

void App::key_callback(int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
    if (ImGui::GetIO().WantCaptureKeyboard)
    {
        return;
    }

    float static speed = -3.0;
     
    if (key == GLFW_KEY_N)
    {
        speed += 1.0;
    }
    if (key == GLFW_KEY_M)
    {
        speed -= 1.0;
    }

    glm::vec3 velocity = glm::vec3(0);

    if (key == GLFW_KEY_W)
    {
        velocity += glm::vec3(0,0,speed);
        // player._camera.moveForward(speed);
    }
    if (key == GLFW_KEY_S)
    {
        velocity += glm::vec3(0,0,-speed);
        // player._camera.moveBack(speed);
    }

    if (key == GLFW_KEY_A)
    {
        velocity += glm::vec3(speed,0,0);
        // player._camera.moveLeft(speed);
    }

    if (key == GLFW_KEY_D)
    {
        velocity += glm::vec3(-speed,0,0);
        // player._camera.moveRight(speed);
    }

    if (key == GLFW_KEY_Y)
    {
        velocity += glm::vec3(0,speed,0);
        // player._camera.moveUp(speed);
    }
    if (key == GLFW_KEY_U)
    {
        velocity += glm::vec3(0,-speed,0);
        // player._camera.moveUp(speed);
        // player._camera.moveDown(speed);
    }
    if (key == GLFW_KEY_I)
    {
       use_physics = false;
    }
    if (key == GLFW_KEY_ESCAPE)
    {
        if (keyDebounce(key))
        {
            _mouseDisabled = !_mouseDisabled;
        }
    }

    player.moveSimple(velocity);
    std::cout << velocity.x << velocity.y << velocity.z << std::endl;
}

void App::mouse_button_callback(int button, int action, int /*mods*/)
{
    if (button == 0 && action == 1)
    {
       std::cout << "click" << std::endl;
    }
    if (button == 1 && action == 1)
    {
       std::cout << "click" << std::endl;
    }
}

void App::scroll_callback(double /*xoffset*/, double /*yoffset*/)
{
}


void App::cursor_position_callback(double xpos, double ypos)
{



    static double prev_xpos = 0;
    static double prev_ypos = 0;
    static bool inited = false;
    double dx = (prev_xpos - xpos);
    double dy = (prev_ypos - ypos);

    if (ImGui::GetIO().WantCaptureMouse || _mouseDisabled)
    {
        prev_xpos = xpos;
        prev_ypos = ypos;
        return;
    }

    if (!inited)
    {
        inited = true;
        prev_xpos = xpos;
        prev_ypos = ypos;
    }

    player._camera.rotateRelative(glm::vec2(dx, dy));

    prev_xpos = xpos;
    prev_ypos = ypos;

}

void App::size_callback(int width, int height)
{
    _width  = width;
    _height = height;
    glViewport(0, 0, width, height);
}
