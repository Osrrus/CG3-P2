#pragma once
#include "../RYDefine.h"

class GBuffer
{
public:
private:
    
};
class SSAO
{
public:
    SSAO();
    ~SSAO();
	enum SSAO_TEXTURE_TYPE {
        SSAO_TEXTURE_TYPE_POSITION,
        SSAO_TEXTURE_TYPE_DIFFUSE,
        SSAO_TEXTURE_TYPE_NORMAL,
        SSAO_TEXTURE_TYPE_TEXCOORD,
        SSAO_NUM_TEXTURES
    };
    bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void BindForWriting();

    void BindForReading();
    void SetReadBuffer(int TextureType);
    //void SetReadBuffer(SSAO_TEXTURE_TYPE TextureType);
    GLuint getFBO();
    GLuint m_textures[SSAO_NUM_TEXTURES];

private:
    GLuint m_fbo;
    GLuint m_depthTexture;
};
