#include "App.hpp"
#include "GLFW/glfw3.h"
#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <glm/glm.hpp>


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

void App::init()
{
    float vertices[] = { // NOLINT
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };  

    _shaders.fragment_name = "src/shaders/shader.frag";
    _shaders.vertex_name = "src/shaders/shader.vert";
    _shaders.program = compile_shader(_shaders);

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

    // glBindVertexArray(VAO);
    _VAO = VAO;
    _VBO = VBO;
}

void App::render()
{
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(_shaders.program);

    double timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    
    float rot[] = {cos(greenValue), -sin(greenValue), 0, 0,
                   sin(greenValue), cos(greenValue),  0, 0,
                   0,               0,                1, 0,
                   0,               0,                0, 1};

    int num = -1;
    // Set uniforms
    int rotateLocation = glGetUniformLocation(_shaders.program, "rotate");
    // glUniform4fv(rotateLocation, 1, rot);
    glUniformMatrix4fv(rotateLocation, 1, false, rot);
    int vertexColorLocation = glGetUniformLocation(_shaders.program, "outColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    glGetProgramiv(_shaders.program, GL_ACTIVE_UNIFORMS, &num);
    std::cout << "Active uniforms \n" << num << std::endl;

    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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
