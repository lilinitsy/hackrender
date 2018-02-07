#ifndef GALAXY_H
#define GALAXY_H

#include "ParticleSystem.h"
#include "Star.h"

class Galaxy : public ParticleSystem
{
    public:
        vector<Star> stars;
        float radius;
        glm::vec3 position; 

        int number_stars;

        Galaxy()
        {

        }

        void init()
        {
            for(int i = 0; i < number_stars)
            {
                
            }
        }
};



#endif