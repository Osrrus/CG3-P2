#include "Mesh.h"
Mesh::MeshEntry::MeshEntry()
{
    /*m_VAO = -1;
    m_VBO = -1;
    m_IBO = -1;
    m_Size = 0;
    materialIndex = -1;*/
}

Mesh::MeshEntry::~MeshEntry()
{
    if (m_VBO)
    {
        glDeleteBuffers(1, &m_VBO);
    }

    if (m_IBO)
    {
        glDeleteBuffers(1, &m_IBO);
    }
}
void printInfo(vector<Vertex> vertices) 
{

    for (int i = 0; i < vertices.size(); i++)
    {
        cout << vertices[i].Position.x << " " << vertices[i].Position.y << " " << vertices[i].Position.z << " " << endl;
        /*cout << vertices[i].TexCoords.x << " " << vertices[i].TexCoords.y << endl;
        cout << vertices[i].Normal.x << " " << vertices[i].Normal.y << " " << vertices[i].Normal.z << " " << endl;*/
    }

}
void Mesh::MeshEntry::init(vector<Vertex> vertices, vector<unsigned int> indices)
{
    m_Size = indices.size();
    /*cout << m_Size << endl;*/
    printInfo(vertices);

    glGenVertexArrays(1, &this->m_VAO);
    glGenBuffers(1, &this->m_VBO);
    glGenBuffers(1, &this->m_IBO);

    glBindVertexArray(this->m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    //cout << this->m_IBO << "  " << this->m_VBO << endl;
    glBindVertexArray(0);
}


Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    Clear();
}

bool Mesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    bool Ret = false;
    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs );

    if (pScene) {
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }

    return Ret;
}

void Mesh::draw()
{
    int m_EntSize = m_Entries.size();
    for (unsigned int i = 0; i < m_EntSize; i++)
    {
        /*glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].m_VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(float)*3));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(float)*5));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].m_IBO);

        /*const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }*/
        glBindVertexArray(m_Entries[i].m_VAO);
        glDrawElements(GL_TRIANGLES, m_Entries[i].m_Size, GL_UNSIGNED_INT, 0);
            //cout << m_Entries[i].m_IBO << "  " << m_Entries[i].m_VBO << endl;
        glBindVertexArray(0);

        /*glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);*/
    }

}

bool Mesh::InitFromScene(const aiScene* scene, const std::string& Filename)
{
    m_Entries.resize(scene->mNumMeshes);

    //Recorremos los mesh de la escena uno por uno
    int m_EntSize = m_Entries.size();
    cout << m_EntSize << endl;
    for (unsigned int i = 0; i < m_EntSize; i++) {
        const aiMesh* paiMesh = scene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return true;
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal = &(paiMesh->mNormals[i]) ? &(paiMesh->mNormals[i]): &Zero3D;
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
            glm::vec2(pTexCoord->x, pTexCoord->y),
            glm::vec3(pNormal->x, pNormal->y, pNormal->z));

        Vertices.push_back(v);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
        cout << Face.mIndices[0] << " " << Face.mIndices[1] << " " << Face.mIndices[2] << " " << endl;
    }

    m_Entries[Index].init(Vertices, Indices);
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
   
    return true;
}

void Mesh::Clear()
{
}

