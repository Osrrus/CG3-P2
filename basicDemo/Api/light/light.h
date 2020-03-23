#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

class light
{
public:
	light(glm::vec3 lightP);
	light();
	~light();
	struct Color
	{
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};
	glm::vec3 pos;
	glm::vec3 dir;
	void changeONOFF();
	void leftLightDir();
	void rightLightDir();
	Color color;
	bool ON;
private:
};


