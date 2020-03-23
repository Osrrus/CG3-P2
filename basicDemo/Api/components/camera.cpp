#define GLM_ENABLE_EXPERIMENTAL

#include "camera.h"
#include <glm/gtx/transform.hpp>
#include <iostream>

const float MOVEMENT_SPEED = 0.05f;
const float ROTATIONAL_SPEED = 0.006f;
const float SENSITIVITY = 0.1f;
/*
Left eye 
	position(-2.0f,0.0f,3.0f)

Right eye 
	position(2.0f,0.0f,3.0f)
*/

Camera::Camera(glm::vec3 dir, glm::vec3 pos) :
	viewDirection(dir.x, dir.y, dir.z),
	position(pos.x, pos.y, pos.z),
	oldMousePosition(0.0f,0.0f),
	UP(0.0f, 1.0f, 0.0f){
	width = 800;
	height = 600;
	nearZ = 1.0f;
	farZ = 100.0f;
	fov = 90.0f;
}

void Camera::mouseUpdate(const glm::vec2& newMousePosition) {

	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) > 50.0f) {
		oldMousePosition = newMousePosition;
		return;
	}
	
	glm::vec3 toRotateAround = glm::cross(viewDirection, UP);
	glm::mat4 rotator = glm::rotate(-mouseDelta.x*ROTATIONAL_SPEED, UP) *
						glm::rotate(-mouseDelta.y*ROTATIONAL_SPEED, toRotateAround);

	viewDirection = glm::mat3(rotator) * viewDirection;

	oldMousePosition = newMousePosition;
}

glm::mat4 Camera::getWorlToViewMatrix(bool stereo) const{
	if (!stereo)
	{
		return glm::lookAt(position,position + viewDirection, UP);
	}
	else
	{
		return viewMatrix;
	}
}

glm::mat4 Camera::getWorlToProjMatrix(bool stereo) const
{
	if (!stereo)
	{
		return glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);
	}
	else {
		return projectionMatrix;
	}
}

void Camera::moveForward() {

	position += MOVEMENT_SPEED * viewDirection;

}

void Camera::moveBackward() {

	position += -MOVEMENT_SPEED * viewDirection;

}

void Camera::strafeLeft() {

	glm::vec3 strafeDirection = glm::cross(viewDirection, UP);
	position += -MOVEMENT_SPEED * strafeDirection;
}

void Camera::strafeRight() {

	glm::vec3 strafeDirection = glm::cross(viewDirection, UP);
	position += MOVEMENT_SPEED * strafeDirection;
}

void Camera::moveUp() {

	position += MOVEMENT_SPEED * UP;

}

void Camera::moveDown() {

	position += -MOVEMENT_SPEED * UP;

}

void Camera::updateWH(glm::vec2 win)
{
	width = win.x;
	height = win.y;
}

void Camera::stereoViewProjectionMatrices(float IOD, float depthZ, bool leftEye)
{

	glm::vec3 direction_z(0, 0, -1);
	float left_right_direction = -0.5f;
	if (leftEye)
		left_right_direction = 0.5f;
	float aspect_ratio = (float)width / (float)height;
	
	projectionMatrix = glm::perspective(glm::radians(fov/2.0f),aspect_ratio, nearZ, farZ);

	// update the view matrix
	viewMatrix =
		glm::lookAt(
			//eye position
			position - viewDirection +
			glm::vec3(left_right_direction * IOD / 2, 0, 0),
			//centre position
			position +
			glm::vec3(left_right_direction * IOD / 2, 0, 0),
			UP //up direction
		);
	
}