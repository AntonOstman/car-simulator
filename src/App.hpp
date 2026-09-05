#pragma once
#include "UI.hpp"
#include "World.hpp"
#include <glm/ext/matrix_float4x4.hpp>


class App {
public:
private:
    int _width{};
    int _height{};

    UIsettings _UIsettings;
    float _fov = 45.f;
    float _viewDist = 30.f;
    float _cubePos = 20.f;
    bool _mouseDisabled = false;
    World _world;
    RenderingSystem _renderer;

public:
    App(int window_width, int window_height);
    void createEntities();
    void renderGame();
    void gameLoop();
    void createGUI();
    void drawGUI();
    void init();
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_button_callback(int button, int action, int mods);
    void scroll_callback(double xoffset, double yoffset);
    void cursor_position_callback(double xpos, double ypos);
    void size_callback(int width, int height);
    void updateSystems();
    void gameUpdate();
};
