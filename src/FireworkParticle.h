#ifndef FIREWORKPARTICLE_H
#define FIREWORKPARTICLE_H

class FireworkParticle : public Particle
{
    public:
        FireworkParticle() : Particle()
        {

        }

        FireworkParticle(glm::vec3 p, glm::vec3 s, glm::vec3 c, glm::vec3 t, glm::vec3 v, float l, float a) : 
            Particle(p, s, c,  t, v, l, a)
        {
            
        }

        void set_particle_colour(float a)
        {
            int random_colour = rand() % 3;
    /*        float rand_col_one = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float rand_col_two = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            float rand_col_three = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
*/
            if(random_colour == 0)
            {
                colour = glm::vec3(1.0f, 1.0f, 0.0f);
            }

            else if(random_colour == 1)
            {
                colour = glm::vec3(0.0f, 1.0f, 1.0f);
            }

            else
            {
                colour = glm::vec3(1.0f, 0.0f, 1.0f);
            }

            alpha = a;
        }

    
        void update_particle(float delta_time)
        {
            position += velocity * delta_time;
            time_lived += delta_time;
            colour = glm::vec3(colour.x - 0.005f * lifetime, colour.y - 0.005f * lifetime, colour.z - 0.005f * lifetime);
        }
};


#endif