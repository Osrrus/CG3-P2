#pragma once
#include "RYDefine.h"

class RYGraphics
{
    private:
        float framesPerSecond,lastTime;
        int fps;
        float currentTime;
        
    public:
        Camera *camera;
        RYGraphics();
        ~RYGraphics();
        int getFPS();
};


