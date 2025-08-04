#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Loader.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float2.hpp"

std::vector<Vertex> orderVertices(std::string filename, Vertices verts);

std::vector<Vertex> parseObj(std::string filename)
{
    std::ifstream file;
    file.open(filename);
    if (!file.is_open())
    {
        std::cout << "Could not open file" << std::endl;
        exit(1);
    }

    std::vector<glm::vec3> vecVert;
    std::vector<glm::vec3> vecVnorm;
    std::vector<glm::vec2> vecTex;
    Vertices data;
    std::string line;
    while(std::getline(file, line))
    {

        std::stringstream stream(line);
        std::string type = "";
        float x,y,z = 0.0;
        stream >> type >> x >> y >> z;

        if (type == "#")
        {
            // Comment do nothing
        }
        else if(type == "v")
        {
            vecVert.push_back(glm::vec3(x,y,z));
        }
        else if(type == "vt")
        {
            vecTex.push_back(glm::vec2(x,y));
        }
        else if(type == "vn")
        {
            vecVnorm.push_back(glm::vec3(x,y,z));
        }
    }

    data.Pos = vecVert;
    data.Normal = vecVnorm;
    data.TexCoord = vecTex;


    file.close();
    return orderVertices(filename, data);

    // return data;
}

std::vector<Vertex> orderVertices(std::string filename, Vertices verts)
{
    std::ifstream file;
    file.open(filename);
    if (!file.is_open())
    {
        std::cout << "Could not open file" << std::endl;
        exit(1);
    }

    std::string line;
    std::vector<Vertex> vertices;

    while(std::getline(file, line))
    {

        std::stringstream stream(line);
        std::string type, x,y,z = "";

        stream >> type >> x >> y >> z;
        std::string faces[3] = {x,y,z};

        if (type == "#")
        {
            // Comment do nothing
        }
        else if (type == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                Vertex cur_vertex;
                // std::string face = faces[i];

                std::stringstream facestream(faces[i]);
                std::string parsedint;

                std::getline(facestream, parsedint, '/');
                unsigned int v_idx = std::stoi(parsedint) - 1;
                std::getline(facestream, parsedint, '/');
                unsigned int vt_idx = std::stoi(parsedint) - 1;
                std::getline(facestream, parsedint, ' ');
                unsigned int vn_idx = std::stoi(parsedint) - 1;

                cur_vertex.Pos = verts.Pos[v_idx];
                cur_vertex.TexCoord = verts.TexCoord[vt_idx];
                cur_vertex.Normal = verts.Normal[vn_idx];
                std::cout << v_idx << vt_idx << vn_idx << std::endl;

                vertices.push_back(cur_vertex);
            }
        }

    }
    file.close();
    int test_idx = 0;
    std::cout << "start bug "<< std::endl;
    std::cout << "pos: " << vertices[test_idx].Pos.x << ", " << vertices[test_idx].Pos.y << ", "<< vertices[test_idx].Pos.z << std::endl;
    std::cout << "norm: " << vertices[test_idx].Normal.x << ", "<< vertices[test_idx].Normal.y << ", "<< vertices[test_idx].Normal.z << std::endl;
    std::cout << "tex: " << vertices[test_idx].TexCoord.x << ", "<< vertices[test_idx].TexCoord.y << std::endl;
    std::cout << "end bug "<< std::endl;

    return vertices;
}
