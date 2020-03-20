#include "particleSystem.h"

#define PARTICLENUMBER 200

particleSystem::particleSystem():position(0.0f),direction(0.0f,0.25f,0.0f),color(0.2f,0.25f,0.5f,1.0f)
{
	this->spawParticle = 2;
	this->numberOfparticles = PARTICLENUMBER;
	this->active = true;
	this->lastActiveParticle = 0;
	shader = new Shader("Api/particle/shader/particle.vert","Api/particle/shader/particle.frag");
	
	float vertex[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};

	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	for (unsigned int i = 0; i < PARTICLENUMBER; i++) {

		this->particles.push_back(new particle());
	}
}

particleSystem::~particleSystem()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	delete this->shader;
}

void particleSystem::draw(float delta, glm::mat4 view, glm::mat4 projection)
{	
	if(this->active)
		this->createNewParticles();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	this->shader->use();
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);

	for (unsigned int i = 0; i < PARTICLENUMBER; i++) {

		if (this->particles[i]->lifeTime > 0.0f) {

			this->particles[i]->position += this->particles[i]->direction * delta;
			this->particles[i]->color += 0.005 * delta;
			this->particles[i]->draw(this->shader,this->VAO);
			this->particles[i]->lifeTime -= delta;

		}

	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void particleSystem::createNewParticles()
{
	for (unsigned int i = 0; i < spawParticle; i++) {

		particle* p = getFirtsDeathParticle();
		p->position = this->position;
		p->direction = this->direction;
		p->color = this->color;
		p->lifeTime = 4.0f;
		
	}


}

particle* particleSystem::getFirtsDeathParticle() {

	particle* p;

	if (this->lastActiveParticle < this->numberOfparticles) {

		if (this->numberOfparticles > this->particles.size()) {

			if (this->lastActiveParticle >= this->particles.size()) {

				this->particles.push_back(new particle());
		
			}
			
		}

	}
	else {

		this->lastActiveParticle = 0;

	}

	p = this->particles[this->lastActiveParticle];
	this->lastActiveParticle++;
	
	return  p;

}