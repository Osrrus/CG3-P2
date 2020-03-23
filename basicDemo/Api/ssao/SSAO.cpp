#define GLM_ENABLE_EXPERIMENTAL

#include "SSAO.h"

SSAO::SSAO()
{

}

SSAO::~SSAO()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_textures[0] != 0) {
        glDeleteTextures(SSAO_NUM_TEXTURES, m_textures);
    }

    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
    }
}

bool SSAO::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    // Create the gbuffer textures
    glGenTextures(SSAO_NUM_TEXTURES, m_textures);
    //glGenTextures(1, &m_depthTexture);

    for (unsigned int i = 0; i < SSAO_NUM_TEXTURES; i++) {
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i], 0);
    }
    GLenum attachments[SSAO_NUM_TEXTURES] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(SSAO_NUM_TEXTURES, attachments);

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

void SSAO::BindForWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

}

void SSAO::BindForReading()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

}

void SSAO::SetReadBuffer(int TextureType)
{
    glReadBuffer(GL_COLOR_ATTACHMENT0 + TextureType);
}

GLuint SSAO::getFBO()
{
    return m_fbo;
}
