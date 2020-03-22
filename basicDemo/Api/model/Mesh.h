#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;
struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
    //// tangent
    //glm::vec3 Tangent;
    //// bitangent
    //glm::vec3 Bitangent;

    Vertex() {}
    Vertex(glm::vec3 Pos, glm::vec2 TexC, glm::vec3 Nor) {
        Position = Pos;
        Normal = Nor;
        TexCoords = TexC;
    }
};
struct Texture {
    unsigned int id;
    string type;
};

class Mesh
{
public:
    Mesh();

    ~Mesh();

    bool LoadMesh(const std::string& Filename);

    void draw();

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
        MeshEntry();
        ~MeshEntry();
        void init(vector<Vertex> vertices, vector<unsigned int> indices);

    };

    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    string m_filename;
    std::vector<MeshEntry> m_Entries;
    //std::vector<Texture*> m_Textures;
};