#pragma once
#include "worldSystem.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#define GLFW_INCLUDE_NONE
#include "EntityComponentSystem.hpp"


class App {
public:
private:
    int _width{};
    int _height{};
    float _fov = 45.f;
    float _viewDist = 30.f;
    bool _mouseDisabled = false;
    ECS _ecs = ECS();
    RenderingSystem _renderingSystem;
    PhysicsSystem _physicsSystem;
    WorldSystem _worldSystem;

public:
    App(int window_width, int window_height);
    void createEntities();
    void renderGame();
    void createGUI();
    void drawGUI();
    void init();
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_button_callback(int button, int action, int mods);
    void scroll_callback(double xoffset, double yoffset);
    void cursor_position_callback(double xpos, double ypos);
    void size_callback(int width, int height);
    void updateSystems();
};
