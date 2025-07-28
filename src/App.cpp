#include "App.hpp"
#include "GLFW/glfw3.h"
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
#include "Math.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_float4.hpp"

struct shaderInfo {
    std::string vertex_name;
    std::string fragment_name;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
};

glm::mat4 cuberot;
glm::mat4 cameraRot = glm::mat4(1.0);
glm::mat4 cameraTrans = glm::mat4(1.0);

Model model;
Model model2;
Model model3;
Model model4;
Model model5;
GLuint program1 = 0;
GLuint program2 = 0;

App::App(int window_width, int window_height)
{
    size_callback(window_width, window_height);
}

std::string read_file(std::string &filename){
    
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
    // std::cout << buffer << std::endl;
    // std::cout << "^^ outputted filed ^^" << std::endl;

    return buffer;
}

unsigned int compile_shader(shaderInfo &shaders){
    
    std::string frag = read_file(shaders.fragment_name);
    std::string vert = read_file(shaders.vertex_name);

    shaders.vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vert_c_str = vert.c_str();
    glShaderSource(shaders.vertexShader, 1, &vert_c_str, nullptr);
    glCompileShader(shaders.vertexShader);

    shaders.fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* frag_c_str = frag.c_str();
    glShaderSource(shaders.fragmentShader, 1, &frag_c_str, nullptr);
    glCompileShader(shaders.fragmentShader);

    int  success = 0;
    char infoLog[512]; // NOLINT

    glGetShaderiv(shaders.vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaders.vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    }

    glGetShaderiv(shaders.fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaders.fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    }

    unsigned int shaderProgram = 0;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, shaders.vertexShader);
    glAttachShader(shaderProgram, shaders.fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::LINKING::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(1);
    }
    glDeleteShader(shaders.vertexShader);
    glDeleteShader(shaders.fragmentShader);  

    return shaderProgram;
}

void App::init()
{

    float vertices[] = { // NOLINT
        -0.5f, 0.0f, -0.5f,
         0.5f, 0.0f, -0.5f,
         0.5f, 0.0f,  0.5f
    };  

    float floor[] = { // NOLINT
        -1.0f, 0.0f, 1.0f,
         -1.0f, 0.0f, -1.0f,
         1.0f, 0.0f,  -1.0f,

         1.0f, 0.0f,  1.0f,
         -1.0f, 0.0f,  1.0f,
         1.0f, 0.0f,  -1.0f
    };

    float cube[] = { // NOLINT
        // Floor
        -1.0f,  -1.0f, 1.0f,
         -1.0f, -1.0f, -1.0f,
         1.0f,  -1.0f, -1.0f,
                    
         1.0f,  -1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,
         1.0f,  -1.0f,  -1.0f,

        // Roof
        -1.0f,  1.0f, 1.0f,
         -1.0f, 1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,

         1.0f,  1.0f,  1.0f,
         -1.0f, 1.0f,  1.0f,
         1.0f,  1.0f,  -1.0f,

    };


    shaderInfo shaders;

    shaders.fragment_name = "src/shaders/shader.frag";
    shaders.vertex_name = "src/shaders/shader.vert";
    program1 = compile_shader(shaders);
    shaders.fragment_name = "src/shaders/shaderBlack.frag";
    program2 = compile_shader(shaders);

    model.generateBuffers(floor, 6);
    model2.generateBuffers(vertices, 3);
    model3.generateBuffers(cube, 3 * 4);
    model4.generateBuffers(cube, 3 * 4);
    model5.generateBuffers(cube, 3 * 4);
    cuberot = glm::mat4(1.0);
}

void bindAndDrawModel(Model& model, const GLuint& program, const glm::mat4& mvp, const float& color)
{

    // Set uniforms and draw
    model.bind(program);
    int mvpLocation = glGetUniformLocation(program, "mvp");
    glUniformMatrix4fv(mvpLocation, 1, false, &mvp[0][0]);

    int vertexColorLocation = glGetUniformLocation(program, "outColor");
    glUniform4f(vertexColorLocation, 0.0f, color, 0.0f, 1.0f);
    model.draw();
}

glm::mat4 Rz(float angle){
    angle = glm::radians(angle);
    glm::mat4 rot = glm::mat4(cos(angle), -sin(angle), 0.0, 0.0,
                              sin(angle), cos(angle),  0.0, 0.0,
                              0.0,           0.0,      1.0, 0.0,
                              0.0,           0.0,      0.0, 1.0);
    return rot;
}

glm::mat4 Rx(float angle){
    angle = glm::radians(angle);
    glm::mat4 rot = glm::mat4(  1.0,    0.0,      0.0,        0.0,
                                0.0, cos(angle), -sin(angle), 0.0,
                                0.0, sin(angle), cos(angle),  0.0,
                                0.0,      0.0,      0.0,      1.0);
    return rot;
}

glm::mat4 Ry(float angle){
    angle = glm::radians(angle);
    glm::mat4 rot = glm::mat4(cos(angle),  0.0, sin(angle),  0.0,
                              0.0,         1.0,   0.0,       0.0,
                              -sin(angle), 0.0,  cos(angle), 0.0,
                              0.0,         0.0,   0.0,       1.0);
    return rot;
}

glm::mat4 scaled_eye(float scale){
    glm::mat4 eye = glm::mat4(scale, 0.0,    0.0,   0.0,
                              0.0,   scale,  0.0,   0.0,
                              0.0,   0.0,    scale, 0.0,
                              0.0,   0.0,    0.0,   1.0);
    return eye;
}

void drawCube(glm::mat4 modelToWorld, glm::mat4 worldToPerspective){

    // top and bottom
    float color = 0.0;
    glm::mat4 mvp = worldToPerspective * modelToWorld;
    bindAndDrawModel(model3, program2, mvp, color);

    // Rotate top and bottom to create a cube
    // first side
    mvp = worldToPerspective * modelToWorld * Rx(90.0);
    bindAndDrawModel(model4, program2, mvp, color);

    // second side
    mvp = worldToPerspective * modelToWorld * Ry(90.0) * Rx(90.0);;
    bindAndDrawModel(model5, program2, mvp, color);
    // ----- Cube -----

}

void App::render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    double timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    float angle = greenValue;
    cuberot = Ry(glm::degrees(angle));
    greenValue = 1.0;

    glm::vec3 up = glm::vec3(0,1,0);
    glm::vec3 center = glm::vec3(0,0,0);
    glm::vec3 eye = glm::vec3(0,5,-10);
    glm::mat4 lookAt = glm::lookAt(eye, center, up);
    glm::mat4 view = cameraRot * lookAt * cameraTrans * cuberot;
    glm::mat4 perspective = glm::perspectiveFov(glm::radians(45.0f), (float) _width, (float) _height, 0.1f, 20.0f);

    glm::mat4 modelToWorld = scaled_eye(1000.0);
    glm::mat4 mvp = perspective * view * modelToWorld;

    // ----- floor -----
    bindAndDrawModel(model, program1, mvp, greenValue);
    // ----- floor -----
    modelToWorld = scaled_eye(1.0);
    // ----- Cube -----
    drawCube(modelToWorld, perspective * view);
    drawCube(glm::translate(modelToWorld, glm::vec3(0,3.1,0)), perspective * view);
    drawCube(glm::translate(modelToWorld, glm::vec3(3,0,0)), perspective * view);
    // ----- Cube -----
}


void App::key_callback(int key, int /*scancode*/, int /*action*/, int /*mods*/)
{
    if (key == GLFW_KEY_W)
    {
        cameraTrans = glm::translate(cameraTrans, glm::vec3(0, 0, -0.1));
    }
    if (key == GLFW_KEY_S)
    {
        cameraTrans = glm::translate(cameraTrans, glm::vec3(0, 0, 0.1));
    }

    if (key == GLFW_KEY_A)
    {
        cameraTrans = glm::translate(cameraTrans, glm::vec3(-0.1, 0, 0));
    }

    if (key == GLFW_KEY_D)
    {
        cameraTrans = glm::translate(cameraTrans, glm::vec3(0.1, 0, 0));
    }
}

void App::mouse_button_callback(int /*button*/, int /*action*/, int /*mods*/)
{
}

void App::scroll_callback(double /*xoffset*/, double /*yoffset*/)
{
}

double prev_xpos = 0;
double prev_ypos = 0;
double cury = 0;
double curx = 0;
bool inited = false;

void App::cursor_position_callback(double xpos, double ypos)
{
    if (!inited)
    {
        inited = true;
        prev_xpos = xpos;
        prev_ypos = ypos;
    }

    // camera = glm::rotate(const mat<4, 4, T, Q> &m, T angle, const vec<3, T, Q> &axis)
    cury += prev_ypos - ypos;
    curx += prev_xpos - xpos;
    if (abs(cury) > 40)
    {
        cury -= prev_ypos - ypos;
    }
    // if (abs(curx) > 20)
    // {
    //     curx -= prev_xpos - xpos;
    // }
    // glm::vec3 upbefore = glm::vec3(cameraRot[1]);

    // cameraRot = glm::rotate(glm::mat4(1.0), (float)glm::radians(cury), glm::vec3(1, 0, 0));
    // glm::vec3 upafter = glm::vec3(cameraRot[1]);
    // cameraRot = glm::rotate(cameraRot, (float)glm::radians(curx), upafter);

    glm::quat pitch = glm::angleAxis((float)glm::radians(cury), glm::vec3(1,0,0));
    glm::quat yaw = glm::angleAxis((float)glm::radians(curx), glm::vec3(0,1,0));
    glm::quat rot = pitch * yaw;

    cameraRot = glm::mat4(rot);
    // cameraRot[1] = pitch * cameraRot[1];
    // cameraRot[0] = yaw * cameraRot[0];

    prev_xpos = xpos;
    prev_ypos = ypos;
}

void App::size_callback(int width, int height)
{
    _width  = width;
    _height = height;
}
