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
		bool left;
        float IOD;
		Camera *camera;
		/* Esteroscopy */
		Camera *rightEye;
		Camera *leftEye;

        RYGraphics();
        ~RYGraphics();

        int getFPS();
        float getDeltaTime();
};


