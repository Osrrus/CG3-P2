#include "RYGraphics.h"



RYGraphics::RYGraphics()
{
    this->camera = new Camera();
    this->framesPerSecond = 0.0f;
    this->lastTime = glfwGetTime();
    this->framesPerSecond = 0.0f;
    this->lastTime = glfwGetTime();
    this->stereoscopy = false;
    //this->IOD = ;
	this->left = true;
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

void setCameraStereoscopy() 
{

}

float RYGraphics::getDeltaTime() {

    return currentTime - lastTime;
}