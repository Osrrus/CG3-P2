#include "RYGraphics.h"



RYGraphics::RYGraphics()
{
    this->camera = new Camera();
	this->rightEye = new Camera(glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(2.0f, 0.0f, 3.0f));
	this->leftEye = new Camera(glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3(-2.0f, 0.0f, 3.0f));
    this->framesPerSecond = 0.0f;
    this->lastTime = 0.0f;
	this->stereoscopy = false;
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