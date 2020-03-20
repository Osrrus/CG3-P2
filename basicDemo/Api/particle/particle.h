#pragma once

#include "../RYDefine.h"

class particle
{
    private:
       

    public:
        glm::vec3 position, scale, direction;
        glm::vec4 color;
        float lifeTime;
        particle();
        ~particle();
        void draw(Shader*,unsigned int);
        void lookAt();
        unsigned int VBO, VAO;
};


