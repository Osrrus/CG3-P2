#pragma once

#include "../RYDefine.h"
#include "particle.h"
#include "../components/loadTexture.h"
#include <vector>
#include <stdlib.h>
#include <time.h> 

class particleSystem
{
    private:
        std::vector<particle*>particles;
        
        unsigned int lastActiveParticle;
        Shader* shader;
        unsigned int VBO[2], VAO, texture;
        float newParticle;

    public:
        bool active;
        int spawParticle;
        glm::vec3 position, direction, scale;
        glm::vec4 color;
        float spawTime, LifeTime;
        int numberOfparticles;
        particleSystem();
        ~particleSystem();
        void draw(float,glm::mat4,glm::mat4, glm::vec3);
        void createNewParticles(float delta);
        particle* getFirtsDeathParticle();
        
};


