#pragma once
#include "../RYDefine.h"

#include "../components/loadTexture.h"
#include "../components/Helper.h"
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
    string path;
    string name;
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

    string SplitFilename(const std::string& str);

    bool LoadMesh(const std::string& Filename);

    void draw(Shader* shader);

    void setModelInOneMesh(int n, glm::vec3 t, glm::vec3 r, glm::vec3 s);


    Texture text;
    bool hasText;
    string m_filename;

    struct MeshEntry
    {
        GLuint m_VAO;
        GLuint m_VBO;
        GLuint m_IBO;
        int m_Size;
        unsigned int materialIndex;
        Texture* text;
        glm::vec3 trans;
        glm::vec3 rotate;
        glm::vec3 scale;
        glm::mat4 model;
        MeshEntry();
        ~MeshEntry();
        void init(vector<Vertex> vertices, vector<unsigned int> indices);

    };
    std::vector<MeshEntry> m_Entries;
private:
    bool InitFromScene(const aiScene* scene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();



    //std::vector<Texture*> m_Textures;
};