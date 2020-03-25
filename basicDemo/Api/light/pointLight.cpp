#include "pointLight.h"

pointLight::pointLight(glm::vec3 lightP)
{
	pos = (lightP);
	setKAttenuation(glm::vec3(0.5f, 0.1f, 0.01f));
	color.diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	ON = true;
	meshL = new Mesh();
	meshL->LoadMesh("assets/models/pointlight.obj");

}

pointLight::~pointLight()
{
}

void pointLight::setKAttenuation(glm::vec3 K)
{
	k = K;
}

glm::vec3 pointLight::getKAttenuation()
{
	return k;
}

void pointLight::draw(Shader* shader)
{
	glm::mat4 model = glm::translate(glm::mat4(1.0f), this->pos) * glm::scale(glm::mat4(1.0f), glm::vec3(5.5f));
	//model = glm::scale();
	meshL->setModelInOneMesh(0, model);
	//set color
	shader->use();
	shader->setVec3("colorDiff", color.diffuse);
	meshL->draw(shader);
}
