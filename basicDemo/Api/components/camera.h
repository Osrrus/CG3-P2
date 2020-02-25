#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {

	const glm::vec3 UP;
	glm::vec2 oldMousePosition;
	int mode;
	/* mode -> 0 normal mode -> 1 stereo_left mode -> 2 stereo_left */
	int IOD;
	int width, height;
	float nearZ;
	float farZ;
	float fov;

	public:
		glm::vec3 viewDirection;	
		Camera(glm::vec3 dir = glm::vec3(0.0f, 0.0f, -4.0f), glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f));
		void mouseUpdate( const glm::vec2& newMousePosition);
		glm::mat4 getWorlToViewMatrix(bool stereo) const;
		glm::mat4 getWorlToProjMatrix(bool stereo) const;
		glm::vec3 position;

		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		glm::mat4 modelMatrix;

		void moveForward();
		void moveBackward();
		void strafeLeft();
		void strafeRight();
		void moveUp();
		void moveDown();
		void updateWH(glm::vec2 win);
		void stereoViewProjectionMatrices(float IOD, float depthZ, bool leftEye);
		
};