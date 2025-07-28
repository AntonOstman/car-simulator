#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#define GLFW_INCLUDE_NONE
#include "glad/glad.h"
#include <string>


class Model;

class App {
public:
private:
    int _width{};
    int _height{};

public:
    App(int window_width, int window_height);
    void render();
    void init();
    void key_callback(int key, int scancode, int action, int mods);
    void mouse_button_callback(int button, int action, int mods);
    void scroll_callback(double xoffset, double yoffset);
    void cursor_position_callback(double xpos, double ypos);
    void size_callback(int width, int height);
};
