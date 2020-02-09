#include "RYGraphics.h"



RYGraphics::RYGraphics()
{
    this->camera = new Camera();
    this->framesPerSecond = 0.0f;
    this->lastTime = 0.0f;
}

RYGraphics::~RYGraphics()
{
}

int RYGraphics::getFPS(){

    currentTime = glfwGetTime();

    ++framesPerSecond;

    if (currentTime - lastTime > 1.0f)
    {
        lastTime = currentTime;
        fps = (int)framesPerSecond;
        framesPerSecond = 0;
    }

    return fps > 60 ?  60 :  fps;

}