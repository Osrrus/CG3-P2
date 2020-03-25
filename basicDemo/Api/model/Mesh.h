#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../components/loadTexture.h"
#include "../Shader.h"
using namespace std;
struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
    //// tangent
    //glm::vec3 Tangent;
    //// bitangent
    //glm::vec3 Bitangent;

    Vertex(glm::vec3 Pos, glm::vec2 TexC, glm::vec3 Nor) {
        Position = Pos;
        Normal = Nor;
        TexCoords = TexC;
    }
};
struct Texture {
    unsigned int id;
    string type;
    Texture();
    ~Texture();
    void load(const char* path) 
    {
        id = loadT(path);
    }
    int bind(int n)
    {
        glActiveTexture(GL_TEXTURE0 + n);
        glBindTexture(GL_TEXTURE_2D, id);
        return n;
    }
};

class Mesh
{
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string& Filename);

    void draw(Shader* shader);

    void setModelInOneMesh(int n, glm::mat4 model);


    Texture text;

private:
    bool InitFromScene(const aiScene* scene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

    struct MeshEntry
    {
        GLuint m_VAO;
        GLuint m_VBO;
        GLuint m_IBO;
        int m_Size;
        unsigned int materialIndex;
        unsigned int *texture;
        glm::mat4 model;
        MeshEntry();
        ~MeshEntry();
        void init(vector<Vertex> vertices, vector<unsigned int> indices);

    };


    string m_filename;
    std::vector<MeshEntry> m_Entries;
    //std::vector<Texture*> m_Textures;
};