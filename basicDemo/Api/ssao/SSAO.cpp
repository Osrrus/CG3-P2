#include "SSAO.h"

SSAO::SSAO()
{
    VAO = VBO = 0;
    radius = 0.5f;
    bias = 0.025f;
    kernelSize = 32;
    noiseScale = glm::vec2(800.0 / 4.0, 600.0 / 4.0);

}

SSAO::~SSAO()
{
    if (FBO != 0) {
        glDeleteFramebuffers(1, &FBO);
    }
    if (blurFBO != 0) {
        glDeleteFramebuffers(1, &blurFBO);
    }

    if (noiseTexture != 0) {
        glDeleteTextures(1, &noiseTexture);
    }
    if (ssaoColorBuffer != 0) {
        glDeleteTextures(1, &ssaoColorBuffer);
    }
    if (ssaoColorBufferBlur != 0) {
        glDeleteTextures(1, &ssaoColorBufferBlur);
    }
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &VAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &VBO);
    ssaoKernel.clear();
    ssaoNoise.clear();
}

bool SSAO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    glGenFramebuffers(1, &FBO);  glGenFramebuffers(1, &blurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
        return false;
    }
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
    
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    initKernel();
    initNosieText();

    return true;
}


void SSAO::initKernel()
{
    ssaoKernel.clear();
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < kernelSize; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / (float)kernelSize;

        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
}

void SSAO::initNosieText()
{
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

float SSAO::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void SSAO::genSSAOText(Shader *shader, Shader* shaderBlur, glm::mat4 proj, GLuint gPosition, GLuint gNormal)
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_COLOR_BUFFER_BIT);
    shader->use();
    for (unsigned int i = 0; i < kernelSize; ++i)
    {
        shader->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    }
    shader->setInt("kernelSize", kernelSize);
    shader->setFloat("bias", bias);
    shader->setFloat("radius", radius);
    shader->setVec2("noiseScale", noiseScale);
    shader->setMat4("projection", proj);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    renderQuad(VAO, VBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    shaderBlur->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    renderQuad(VAO, VBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::lightPass(Shader* shaderLight, light* dirLight, std::vector<pointLight*> pLight, int N_L, GLuint *m_textures, glm::mat4 view) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderLight->use();
    shaderLight->setInt("gPosition", 0);
    shaderLight->setInt("gDiffuse", 1);
    shaderLight->setInt("gNormal", 2);
    shaderLight->setInt("SSAO", 3);
    for (unsigned int i = 0; i < 3; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);
    }
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    /*glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glActiveTexture(GL_TEXTURE0 + 5);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);*/

    //Directional light
    shaderLight->setVec3("dirLight.pos", dirLight->pos);
    shaderLight->setVec3("dirLight.dir", dirLight->dir);
    shaderLight->setVec3("dirLight.ambient", dirLight->color.ambient);
    shaderLight->setVec3("dirLight.diffuse", dirLight->color.diffuse);
    shaderLight->setVec3("dirLight.specular", dirLight->color.specular);
    shaderLight->setBool("dirLight.on", dirLight->ON);
    
    //Point light
    for (int ii = 0; ii < N_L; ii++)
    {
        std::string it = std::to_string(ii);
        glm::vec3 lightPosV = glm::vec3(view * glm::vec4(pLight[ii]->pos, 1.0f));
        shaderLight->setVec3("pointLights[" + it + "].pos", pLight[ii]->pos);
        shaderLight->setVec3("pointLights[" + it + "].posLight", lightPosV);
        shaderLight->setVec3("pointLights[" + it + "].ambientColor", pLight[ii]->color.ambient);
        shaderLight->setVec3("pointLights[" + it + "].diffuseColor", pLight[ii]->color.diffuse);
        shaderLight->setVec3("pointLights[" + it + "].specularColor", pLight[ii]->color.specular);
        shaderLight->setVec3("pointLights[" + it + "].attenuationK", pLight[ii]->getKAttenuation());
        shaderLight->setBool("pointLights[" + it + "].on", pLight[ii]->ON);
    }
    
    renderQuad(VAO, VBO);
}

void SSAO::updateNoiseScale(float winW, float winH)
{
    noiseScale = glm::vec2(winW/4.0f, winH/4.0f);
}
