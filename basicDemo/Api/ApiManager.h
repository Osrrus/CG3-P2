#pragma once
#include "camera.h"

Camera * createCamera(){

    return new Camera();

}

int getFPS(){

    static float framesPerSecond = 0.0f;
    static int fps;
    static float lastTime = 0.0f;
    float currentTime = GetTickCount() * 0.001f;

    ++framesPerSecond;

    if (currentTime - lastTime > 1.0f)
    {
        lastTime = currentTime;
        fps = (int)framesPerSecond;
        framesPerSecond = 0;
    }

    return fps > 60 ?  60 :  fps;
}