#pragma once
#include "particle.h"

particle::particle():
    position(0.0f),direction(0.0f),scale(1.0f),color(0.0f){

    this->lifeTime = 0.0f;

}

particle::~particle()
{
}

void particle::draw(Shader *shader,unsigned int particleVAO, glm::vec3 cameraPos)
{
	shader->setVec4("color", color);

	glm::mat4 model = glm::mat4(1.0f); 
	glm::mat4 rotationMatrix = glm::toMat4(sLookAt(cameraPos - this->position, glm::vec3(0.0f, 1.0f, 0.0f)));

	model = glm::translate(model, this->position);
	model = glm::scale(model, this->scale);
	model *= rotationMatrix;

	shader->setMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);

    glBindVertexArray(particleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

void particle::lookAt()
{
}

glm::quat particle::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if ( glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
		
		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);

}

glm::quat particle::sLookAt(glm::vec3 direction, glm::vec3 desiredUp) {

	if (length2(direction) < 0.0001f)
		return glm::quat();

	// Recompute desiredUp so that it's perpendicular to the direction
	// You can skip that part if you really want to force desiredUp
	glm::vec3 right = cross(direction, desiredUp);
	desiredUp = glm::cross(right, direction);

	// Find the rotation between the front of the object (that we assume towards +Z,
	// but this depends on your model) and the desired direction
	glm::quat rot1 = RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), direction);
	// Because of the 1rst rotation, the up is probably completely screwed up. 
	// Find the rotation between the "up" of the rotated object, and the desired up
	glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::quat rot2 = RotationBetweenVectors(newUp, desiredUp);

	// Apply them
	return rot2 * rot1; // remember, in reverse order.
}