#include "light.h"

light::light(glm::vec3 lightP)
{
	pos = lightP;
	dir = lightP;
	color.ambient = glm::vec3(0.1f);
	color.diffuse = glm::vec3(0.35f);
	color.specular = glm::vec3(0.75f);
	ON = true;
}

light::light()
{
	pos = glm::vec3(5.0f, 5.0f, 10.0f);
	dir = glm::vec3(-3.5f, -3.0f, -3.0f);
	color.ambient = glm::vec3(0.1f);
	color.diffuse = glm::vec3(0.35f);
	color.specular = glm::vec3(0.75f);
	ON = true;
}

light::~light()
{
}

void light::changeONOFF()
{
	ON = !ON;
}

void light::leftLightDir()
{
	dir.x -= 0.1f;
}

void light::rightLightDir()
{
	dir.x += 0.1f;
}
