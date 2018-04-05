#ifndef POINTMASS_H
#define POINTMASS_H


#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"


struct PointMass
{
    glm::vec3 position;
    glm::vec3 velocity;

    PointMass()
    {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    PointMass(glm::vec3 pos)
    {
        position = pos;
        velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void to_string()
    {
        printf("Position: %f %f %f\n", position.x, position.y, position.z);
    }
};

#endif