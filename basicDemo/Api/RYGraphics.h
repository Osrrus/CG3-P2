#pragma once
#include "RYDefine.h"

class RYGraphics
{
    private:
        float framesPerSecond,lastTime;
        int fps;
        float currentTime;

    public:
        bool stereoscopy;
        bool ssao;
		bool left;
        float IOD;
		Camera *camera;
        RYGraphics();
        ~RYGraphics();

        int getFPS();
        float getDeltaTime();
};


