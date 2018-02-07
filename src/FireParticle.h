#ifndef FIREPARTICLE_H
#define FIREPARTICLE_H

#include "Particle.h"

class FireParticle : public Particle
{
    public:
        FireParticle() : Particle()
        {

        }


        FireParticle(glm::vec3 p, glm::vec3 s, glm::vec3 c, glm::vec3 t, glm::vec3 v, float l, float a) : 
            Particle(p, s, c,  t, v, l, a)
        {

        }

        void update_particle(float delta_time)
        {
            position += velocity * delta_time;
            time_lived += delta_time;

            if(time_lived / lifetime >= 0.5f)
            {
                float grayscale = luminance();
                grayscale -= delta_time / lifetime;
                colour = glm::vec3(grayscale, grayscale, grayscale);
            }
        }

    private:
        float luminance()
        {
            return .3 * colour.r + .59 * colour.g + .11 * colour.b;
        }
};

#endif