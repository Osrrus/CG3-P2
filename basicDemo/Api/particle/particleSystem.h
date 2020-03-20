#pragma once

#include "particle.h"
#include <vector>
#include "../RYDefine.h"

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
        unsigned int VBO, VAO;

    public:
        bool active;
        particleSystem();
        ~particleSystem();
        void draw(float,glm::mat4,glm::mat4);
        void createNewParticles();
        particle* getFirtsDeathParticle();
        
};


