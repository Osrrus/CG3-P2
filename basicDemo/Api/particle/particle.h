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
        void draw(Shader*,unsigned int, glm::vec3);
        void lookAt();
        glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
        glm::quat sLookAt(glm::vec3 direction, glm::vec3 desiredUp);
        unsigned int texture;
};


