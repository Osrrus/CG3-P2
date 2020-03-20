#include "particle.h"

particle::particle():
    position(0.0f),direction(0.0f),scale(1.0f),color(0.0f){

    this->lifeTime = 0.0f;

}

particle::~particle()
{
}

void particle::draw(Shader *shader,unsigned int particleVAO)
{
	shader->setVec4("color", color);
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::translate(model, this->position);
	model = glm::scale(model, this->scale);
	shader->setMat4("model", model);

    glBindVertexArray(particleVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

void particle::lookAt()
{
}
