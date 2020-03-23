#pragma once
#include "../RYDefine.h"

class GBuffer
{
public:
	GBuffer();
	~GBuffer();
    enum GBUFFER_TEXTURE_TYPE {
        GBUFFER_TEXTURE_TYPE_POSITION,
        GBUFFER_TEXTURE_TYPE_DIFFUSE,
        GBUFFER_TEXTURE_TYPE_NORMAL,
        GBUFFER_TEXTURE_TYPE_TEXCOORD,
        GBUFFER_NUM_TEXTURES
    };
    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading();
    void SetReadBuffer(int TextureType);
    //void SetReadBuffer(GBUFFER_TEXTURE_TYPE TextureType);
    GLuint getFBO();
    GLuint m_textures[GBUFFER_NUM_TEXTURES];
private:
    GLuint m_fbo;
    GLuint m_depthTexture;
};