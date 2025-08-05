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
#include "Camera.hpp"
#include "glm/matrix.hpp"
#include "Loader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct shaderInfo {
    std::string vertex_name;
    std::string fragment_name;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
};

glm::mat4 cuberot;
Camera camera;
Camera cameraStill;
bool cameraStillChosen = false;

Model floormodel;

Model xyzlines;

Model realcube;

GLuint program1 = 0;
GLuint program2 = 0;
GLuint program3 = 0;


App::App(int window_width, int window_height)
{
    size_callback(window_width, window_height);
}

std::string read_file(std::string &filename){

    if (filename == "")
    {
        std::cout << "ERROR::App::read_file No filename specified" << std::endl;
        exit(0);
    }
    
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
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << shaders.vertex_name << std::endl << infoLog << std::endl;
        exit(1);
    }

    glGetShaderiv(shaders.fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaders.fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << shaders.fragment_name << std::endl << infoLog << std::endl;
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
        std::cout << "ERROR::LINKING::COMPILATION_FAILED\n" << shaders.fragment_name << " " <<shaders.vertex_name << "\n" << infoLog << std::endl;
        exit(1);
    }
    glDeleteShader(shaders.vertexShader);
    glDeleteShader(shaders.fragmentShader);  

    return shaderProgram;
}

void App::init()
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

    // float xyz[] = { // NOLINT
    //     // Floor
    //     1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    //     0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    //     0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    // };

    shaderInfo shader1;
    shaderInfo shader2;
    shaderInfo shader3;
    shader1.fragment_name = "src/shaders/shader.frag";
    shader1.vertex_name = "src/shaders/shader.vert";
    shader2.fragment_name = "src/shaders/shaderBlack.frag";
    shader2.vertex_name = "src/shaders/shader.vert";
    shader3.fragment_name = "src/shaders/shaderVertTexNorm.frag";
    shader3.vertex_name = "src/shaders/shaderVertTexNorm.vert";

    program1 = compile_shader(shader1);
    program2 = compile_shader(shader2);
    program3 = compile_shader(shader3);

    std::vector<Vertex> cubeverts = parseObj("assets/cube.obj");
    floormodel.generateBuffersVertNormalTex(floorverts);
    realcube.generateBuffersVertNormalTex(cubeverts);

    cuberot = glm::mat4(1.0);

    camera.setTranslationWorld(glm::vec3(0,1,10));
    cameraStill.setTranslationWorld(glm::vec3(0,2,10));

    // WIP: Testing assimp works
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("assets/cube.obj", aiProcess_Triangulate);
    scene->HasCameras();
}

void bindAndDrawModelIndiviual(Model& model, const GLuint& program, const glm::mat4& modelp, const glm::mat4& view, const glm::mat4& projection)
{

    // Set uniforms and draw
    model.bind(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, false, &modelp[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, false, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, false, &projection[0][0]);

    model.drawTriangles();
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

void App::render()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    double timeValue = glfwGetTime();
    float greenValue = sin(timeValue) / 2.0f + 0.5f;
    float angle = greenValue;
    cuberot = Ry(glm::degrees(angle));

    glm::mat4 worldToView;
    if (cameraStillChosen)
    {
        worldToView = glm::inverse(cameraStill.getViewToWorld());
    }
    else
    {
        worldToView = glm::inverse(camera.getViewToWorld());
    }

    glm::mat4 perspective = glm::perspectiveFov(glm::radians(45.0f), (float) _width, (float) _height, 0.1f, 20.0f);
    glm::mat4 modelToWorld = scaled_eye(1000.0);

    bindAndDrawModelIndiviual(floormodel, program3, modelToWorld, worldToView, perspective);
    modelToWorld = cuberot * scaled_eye(1.0);
    bindAndDrawModelIndiviual(realcube, program3, modelToWorld, worldToView, perspective);
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
    std::cout << dx << std::endl;
    std::cout << dy << std::endl;

    camera.rotateRelative(glm::vec2(dx, dy));
    prev_xpos = xpos;
    prev_ypos = ypos;
}

void App::size_callback(int width, int height)
{
    _width  = width;
    _height = height;
}
