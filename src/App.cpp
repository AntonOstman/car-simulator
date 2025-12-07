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
#include "Camera.hpp"
#include "Loader.hpp"
#include "Debug.hpp"

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


Mesh cube;
Shader shader;
Camera camera;

void App::init()
{
    _UIsettings.debugUI = false;
    _UIsettings.drawLines = false;
    shader = RenderingSystem::createShader("src/shaders/shaderVertTexNorm.frag", "src/shaders/shaderVertTexNorm.vert");
    std::vector<Vertex> cube_verts = parseObj("assets/cube.obj");
    camera.setView();
    camera.setPerspective(45, _width,_height,0.1, 30);
    cube = RenderingSystem::createMesh(cube_verts);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("assets/cube.obj", aiProcess_Triangulate);
    scene->HasCameras();
}

App::App(int window_width, int window_height)
{
    size_callback(window_width, window_height);
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
    auto physics = high_resolution_clock::now();
    float render_fps = 1000.f / (duration_cast<milliseconds>(render - start).count());
    float phys_fps = 1000.f / (duration_cast<milliseconds>(physics - render).count());

    glm::mat4 model = scaled_eye(1);
    model[3] = glm::vec4(0,0,_cubePos,1);
    RenderingSystem::setUniforms(shader.program, model, camera.getWorldToView(), camera.getPerspective());
    std::cout << glm::to_string(model) << std::endl;

    RenderingSystem::drawTriangles(cube, shader.program);

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
    // std::cout << time_since_last_press << std::endl;

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

    float static speed = 0.3;
     
    if (key == GLFW_KEY_N)
    {
        speed += 0.1;
    }
    if (key == GLFW_KEY_M)
    {
        speed -= 0.1;
    }

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
    if (key == GLFW_KEY_ESCAPE)
    {
        if (key_debounce(key))
        {
            _mouseDisabled = !_mouseDisabled;
        }
    }
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

    camera.rotateRelative(glm::vec2(dx, dy));

    prev_xpos = xpos;
    prev_ypos = ypos;

}

void App::size_callback(int width, int height)
{
    _width  = width;
    _height = height;
    glViewport(0, 0, width, height);
}
