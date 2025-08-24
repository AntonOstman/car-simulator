#include "EntityComponentSystem.hpp"
#include "UI.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "RenderingSystem.hpp"
#include "Debug.hpp"
#include "Camera.hpp"
#include <iostream>
#include <fstream>

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

unsigned int compile_shader(std::string vertex_name, std::string fragment_name){
    
    std::string frag = read_file(fragment_name);
    std::string vert = read_file(vertex_name);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vert_c_str = vert.c_str();
    glShaderSource(vertexShader, 1, &vert_c_str, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* frag_c_str = frag.c_str();
    glShaderSource(fragmentShader, 1, &frag_c_str, nullptr);
    glCompileShader(fragmentShader);

    int  success = 0;
    char infoLog[512]; // NOLINT

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED " << vertex_name << std::endl << infoLog << std::endl;
        exit(1);
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED " << fragment_name << std::endl << infoLog << std::endl;
        exit(1);
    }

    unsigned int shaderProgram = 0;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::LINKING::COMPILATION_FAILED\n" << fragment_name << " " << vertex_name << "\n" << infoLog << std::endl;
        exit(1);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);  

    std::cout << "Compile shader " << vertex_name << " " << fragment_name << " success" << std::endl;

    return shaderProgram;
}

void RenderingSystem::setUniforms(const GLuint& program, const glm::mat4& modelp, const glm::mat4& view, const glm::mat4& projection)
{
    glUseProgram(program);

    printError("before RenderingSystem::setUniforms");

    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, false, &modelp[0][0]);
    printError("model RenderingSystem::setUniforms");

    glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, false, &view[0][0]);
    printError("view RenderingSystem::setUniforms");

    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, false, &projection[0][0]);
    printError("projection RenderingSystem::setUniforms");
} 

void RenderingSystem::init()
{
    glDepthMask(GL_TRUE);
}

void RenderingSystem::update(ECS& ecs, UIsettings settings)
{

    printError("before RenderingSystem::update");
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printError("after RenderingSystem::update");

    EntityID cameraEntity = ecs.getEntityWithTag("mainCamera");
    CameraComp mainCamera = ecs.getComponent<CameraComp>(cameraEntity);
    glm::mat4 view = CameraSystem::getWorldToView(mainCamera);
    glm::mat4 perspective = mainCamera.perspective;

    std::vector<EntityID> drawables = ecs.intersection_entity_id<Transform, Mesh, ShaderComp>();

    if (drawables.empty())
    {
        std::cout << "No drawables found" << std::endl;
    }

    for (EntityID drawable : drawables)
    {
        Transform transform = ecs.getComponent<Transform>(drawable);
        Mesh mesh = ecs.getComponent<Mesh>(drawable);
        ShaderComp shader = ecs.getComponent<ShaderComp>(drawable);
        glm::mat4 model = transform.modelToWorld;

        RenderingSystem::setUniforms(shader.program, model, view, perspective);
        if (settings.drawLines)
        {
            RenderingSystem::drawLines(mesh, shader.program);
        }
        else
        {
            RenderingSystem::drawTriangles(mesh, shader.program);
        }
    }
}

ShaderComp RenderingSystem::createShader(std::string frag, std::string vert)
{
    ShaderComp shader;
    GLuint program = compile_shader(vert, frag);
    shader.program = program;
    return shader;
}
        
Mesh RenderingSystem::createMesh(const std::vector<Vertex> &vertices)
{
    Mesh mesh;
    GLuint VAO = 0;
    GLuint VBO = 0;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);  
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    mesh.VAO = VAO;
    mesh.VBO = VBO;
    mesh.num_vert = vertices.size();
    printError("Model::generateBuffers");
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Pos)));
    printError("Model::bind::vertex");

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, TexCoord)));
    printError("Model::bind::tex");

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normal)));
    printError("Model::bind::normal");

    glBindVertexArray(0);
    return mesh;
}

void RenderingSystem::drawTriangles(Mesh& mesh, GLuint program)
{
    printError("before RenderingSystem::drawTriangles");
    assert(mesh.VAO != 0 && "Uninitialized VAO");
    assert(program != 0 && "Uninitialized program");
    assert(mesh.num_vert != 0 && "No vertices in mesh");

    glBindVertexArray(mesh.VAO);
    glUseProgram(program);

    printError("RenderingSystem::drawTriangles bind error");
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, mesh.num_vert);
    printError("RenderingSystem::drawTriangles draw error");

    glBindVertexArray(0);
}

void RenderingSystem::drawLines(Mesh& mesh, GLuint program)
{
    printError("before RenderingSystem::drawLines");
    glBindVertexArray(mesh.VAO);
    glUseProgram(program);

    printError("RenderingSystem::drawLines bind error");
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINES, 0, mesh.num_vert);
    printError("RenderingSystem::drawLines draw error");

    glBindVertexArray(0);
}
