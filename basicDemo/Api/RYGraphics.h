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
		Camera *camera;
		/* Esteroscopy */
		Camera *rightEye;
		Camera *leftEye;

        RYGraphics();
        ~RYGraphics();

        int getFPS();
};


