#include "pointLight.h"

pointLight::pointLight(glm::vec3 lightP)
{
	pos = (lightP);
	setKAttenuation(glm::vec3(0.5f, 0.1f, 0.01f));
	color.diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
	ON = true;
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
