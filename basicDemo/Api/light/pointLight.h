#pragma once
#include "../RYDefine.h"
#include "light.h"
#include "../model/Mesh.h"
class pointLight : public light
{
public:
	pointLight(glm::vec3 lightP);
	~pointLight();
	glm::vec3 k; //Coeficientes de atenuación
	void setKAttenuation(glm::vec3 K);
	glm::vec3 getKAttenuation();
	Mesh *meshL;
	void draw(Shader *shader);
private:

};
