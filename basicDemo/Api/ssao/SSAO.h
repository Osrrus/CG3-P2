#pragma once
#include "../RYDefine.h"
#include "../components/Helper.h"
#include "../light/light.h"
#include "../light/pointLight.h"

#include <random>

using namespace std;

class SSAO
{
public:
    SSAO();
    ~SSAO();
	
    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void initKernel();

    void initNosieText();

    float lerp(float a, float b, float f);

    void genSSAOText(Shader* shader, Shader* shaderBlur, glm::mat4 proj, GLuint gPosition, GLuint gNormal);

    void lightPass(Shader* shaderLight, light* dirLight, std::vector<pointLight*> pLight, int N_L, GLuint* m_textures, glm::mat4 view);
    float radius, bias;
private:
    GLuint FBO, blurFBO;
    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    GLuint ssaoColorBuffer, ssaoColorBufferBlur;
    GLuint VAO, VBO;
    GLuint noiseTexture;
};
