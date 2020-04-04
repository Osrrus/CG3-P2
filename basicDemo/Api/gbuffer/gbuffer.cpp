#define GLM_ENABLE_EXPERIMENTAL

#include "gbuffer.h"

GBuffer::GBuffer()
{
    quadVAO = quadVBO = 0;
}

GBuffer::~GBuffer()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_textures[0] != 0) {
        glDeleteTextures(GBUFFER_NUM_TEXTURES, m_textures);
    }

    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
    }
    // Deletes the vertex array from the GPU
    glDeleteVertexArrays(1, &quadVAO);
    // Deletes the vertex object from the GPU
    glDeleteBuffers(1, &quadVBO);
}

bool GBuffer::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    // Create the gbuffer textures
    glGenTextures(GBUFFER_NUM_TEXTURES, m_textures);
    //glGenTextures(1, &m_depthTexture);

    for (unsigned int i = 0; i < GBUFFER_NUM_TEXTURES; i++) {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }
    GLenum attachments[GBUFFER_NUM_TEXTURES] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(GBUFFER_NUM_TEXTURES, attachments);

    //// depth
    //glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
    //    NULL);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
    //unsigned int rboDepth;
    glGenRenderbuffers(1, &m_depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthTexture);
  /*  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);*/

    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("FB error, status: 0x%x\n", status);
        return false;
    }
  /*  glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);*/
    // restore default FBO
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    return true;
}

void GBuffer::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

}

void GBuffer::BindForReading()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

}

void GBuffer::SetReadBuffer(int TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

GLuint GBuffer::getFBO()
{
    return m_fbo;
}

void GBuffer::lightPass(Shader * shaderLight, light *dirLight, std::vector<pointLight*> pLight, int N_L, int windowWidth, int windowHeight, glm::vec3 viewPos) {

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shaderLight->use();
    shaderLight->setInt("gPosition", 0);
    shaderLight->setInt("gDiffuse", 1);
    shaderLight->setInt("gNormal", 2);
    shaderLight->setInt("gTextCoord", 3);
    for (unsigned int i = 0; i < GBuffer::GBUFFER_NUM_TEXTURES; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, this->m_textures[i]);
    }
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
        shaderLight->setVec3("pointLights[" + it + "].pos", pLight[ii]->pos);
        shaderLight->setVec3("pointLights[" + it + "].ambientColor", pLight[ii]->color.ambient);
        shaderLight->setVec3("pointLights[" + it + "].diffuseColor", pLight[ii]->color.diffuse);
        shaderLight->setVec3("pointLights[" + it + "].specularColor", pLight[ii]->color.specular);
        shaderLight->setVec3("pointLights[" + it + "].attenuationK", pLight[ii]->getKAttenuation());
        shaderLight->setBool("pointLights[" + it + "].on", pLight[ii]->ON);
    }
    shaderLight->setVec3("viewPos", viewPos);

    renderQuad(quadVAO, quadVBO);

    this->BindForReading();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

//void GBuffer::renderQuad()
//{
//    if (quadVAO == 0)
//    {
//        float quadVertices[] = {
//            // positions        // texture Coords
//            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
//            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
//            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
//            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//        };
//        // setup plane VAO
//        glGenVertexArrays(1, &quadVAO);
//        glGenBuffers(1, &quadVBO);
//        glBindVertexArray(quadVAO);
//        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//    }
//    glBindVertexArray(quadVAO);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    glBindVertexArray(0);
//}
//
