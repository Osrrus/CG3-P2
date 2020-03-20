#pragma once

#include "../RYDefine.h"
#include "particle.h"
#include "../components/loadTexture.h"
#include <vector>

class particleSystem
{
    private:
        std::vector<particle*>particles;
        int numberOfparticles;
        int spawParticle;
        unsigned int lastActiveParticle;
        glm::vec3 position,direction;
        glm::vec4 color;
        Shader* shader;
        unsigned int VBO[2], VAO, texture;

    public:
        bool active;
        particleSystem();
        ~particleSystem();
        void draw(float,glm::mat4,glm::mat4);
        void createNewParticles(float delta);
        particle* getFirtsDeathParticle();
        
};


