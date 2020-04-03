#include "particleSystem.h"

#define PARTICLENUMBER 2000

particleSystem::particleSystem():position(30.0f,0.0f,0.0f),direction(0.0f,1.0f,0.0f),color(0.2f,0.25f,0.5f,1.0f),scale(2.0f)
{	
	srand(time(NULL));

	this->spawParticle = 4;
	this->numberOfparticles = PARTICLENUMBER;
	this->active = true;
	this->lastActiveParticle = 0;
	this->shader = new Shader("Api/particle/shader/particle.vert","Api/particle/shader/particle.frag");
	this->texture = loadT("assets/textures/p.png");
	this->newParticle = 0.0f;
	this->spawTime = 1.0f;
	this->LifeTime = 10.0f;

	float vertex[] = {
		 0.2f, 0.2f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.2f, 0.0f,

		 0.2f, 0.2f, 0.0f,
	 	 0.2f, 0.0f, 0.0f,
		 0.0f, 0.0f, 0.0f
	};

	float uvs[] = {
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	glGenVertexArrays(1, &VAO);

	glGenBuffers(2, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), &uvs, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	for (unsigned int i = 0; i < PARTICLENUMBER; i++) {

		this->particles.push_back(new particle());
	}
}

particleSystem::~particleSystem()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(2, this->VBO);
	delete this->shader;
}

void particleSystem::draw(float delta, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPos)
{	

	if (this->active && newParticle > this->spawTime) {

		this->createNewParticles(delta);
		newParticle = 0;
	}
	else {
		newParticle += delta;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	this->shader->use();
	shader->setMat4("view", view);
	shader->setMat4("projection", projection);
	
	for (unsigned int i = 0; i < PARTICLENUMBER; i++) {

		if (this->particles[i]->lifeTime > 0.0f) {

			this->particles[i]->position += this->direction *delta;
			this->particles[i]->color += 0.005 * delta;
			this->particles[i]->draw(this->shader,this->VAO,cameraPos);
			this->particles[i]->lifeTime -= delta;

		}

	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void particleSystem::createNewParticles(float delta)
{
	for (unsigned int i = 0; i < spawParticle; i++) {

		particle* p = getFirtsDeathParticle();
		float random = (rand() % 100 - 50) / 10.0f;

		p->position.x = this->position.x > 0 ? rand() % 3 + this->position.x + random : 0.0f + random;
		p->position.y = this->position.y > 0 ? rand() % 3 + this->position.y + random : 0.0f + random;
		p->position.z = this->position.z > 0 ? rand() % 3 + this->position.z + random : 0.0f + random;


		p->direction.x = this->direction.x > 0 ? rand() % 3 + this->direction.x + random : 0.0f;
		p->direction.y = this->direction.y > 0 ? rand() % 3 + this->direction.y + random : 0.0f;
		p->direction.z = this->direction.z > 0 ? rand() % 3 + this->direction.z + random : 0.0f;

		p->color = this->color;
		p->scale = this->scale;
		p->lifeTime = this->LifeTime;
		p->texture = this->texture;
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