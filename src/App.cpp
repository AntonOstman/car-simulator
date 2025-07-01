#include "App.hpp"
#include "GLFW/glfw3.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <array>
#include <filesystem>
namespace fs = std::filesystem;
using shaderInfo = struct {
    std::string vertex_name;
    std::string fragment_name;
    GLuint vertexShader;
    GLuint fragmentShader;
};


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
    std::cout << buffer << std::endl;
    std::cout << "^^ outputted filed ^^" << std::endl;

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

void App::render()
{

    float vertices[] = { // NOLINT
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };  

    // glClearColor(1.f, 0.5f, 0.5f, 1.f);
    // glClear(GL_COLOR_BUFFER_BIT);
    shaderInfo shaders;

    shaders.fragment_name = "src/shaders/shader.frag";
    shaders.vertex_name = "src/shaders/shader.vert";
    GLuint program = compile_shader(shaders);

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    glGenVertexArrays(1, &VAO);  
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);  
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    double timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

    int vertexColorLocation = glGetUniformLocation(program, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    
    float rot[] = {cos(greenValue), -sin(greenValue),0,0,
                   sin(greenValue), cos(greenValue),0,0,
                   0,0,1,0,
                   0,0,0,1};

    // float rot[] = {cos(greenValue), 0,-sin(greenValue),0,
    //                0, 1,0,0,
    //                sin(greenValue),0,cos(greenValue),0,
    //                0,0,0,1};
    // float a = greenValue
    // float rot[] = {1,   0,         0,   0,
    //                0,   cos(a), -sin(a),0,
    //                0,   sin(a), cos(a), 0,
    //                0,    0,       0,    1};

    GLint MatrixID = glGetUniformLocation(program, "rotate");
    glUniform4fv(MatrixID, 1, rot);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glUseProgram(program);
    // 3. now draw the object 
    // someOpenGLFunctionThatDrawsOurTriangle();   

}

void App::key_callback(int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
}

void App::mouse_button_callback(int /*button*/, int /*action*/, int /*mods*/)
{
}

void App::scroll_callback(double /*xoffset*/, double /*yoffset*/)
{
}

void App::cursor_position_callback(double /*xpos*/, double /*ypos*/)
{
}

void App::size_callback(int width, int height)
{
    _width  = width;
    _height = height;
}
