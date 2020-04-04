#pragma once

#include "../RYDefine.h"

class bloom
{
    private:
        
    public:
        unsigned int hdrFBO;
        unsigned int colorBuffers[2];

        bloom();
        ~bloom();
        void setBloom();
};


