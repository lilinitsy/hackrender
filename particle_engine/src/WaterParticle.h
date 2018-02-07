#ifndef WATERPARTICLE_H
#define WATERPARTICLE_H

#include "Particle.h"

class WaterParticle : public Particle
{
    public:
        glm::vec3 acceleration;

        WaterParticle() : Particle()
        {
            acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
        }

        WaterParticle(glm::vec3 p, glm::vec3 s, glm::vec3 c, glm::vec3 t, glm::vec3 v, float l, float a) : 
            Particle(p, s, c,  t, v, l, a)
        {
            acceleration = glm::vec3(0.0f, -9.8f, 0.0f);
        }

		
        void set_particle_velocity(glm::vec3 mean_velocity, glm::vec3 variance_velocity)
        {
            
            float velocity_temp_x = (-1.0f * mean_velocity.x) + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (mean_velocity.x - -1.0f * mean_velocity.x)));
            float velocity_temp_z = (-1.0f * mean_velocity.z) + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (mean_velocity.z - -1.0f * mean_velocity.z)));
            float magnitude_velocity = abs(velocity_temp_x) + abs(velocity_temp_z);

            float velocity_x = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / magnitude_velocity));
            float velocity_y = mean_velocity.y + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_velocity.y));            
            float velocity_z = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / magnitude_velocity - velocity_x));

            velocity = glm::vec3(velocity_x, velocity_y, velocity_z);
        }


        void update_particle(float delta_time)
        {
            if(position.y <= 0.0f)
			{
				velocity.y *= -0.95f;
				velocity.x *= 0.45f;
				velocity.z *= 0.45f;
			}
            
            velocity += acceleration * delta_time;
            position += velocity * delta_time;
            time_lived += delta_time;
        }
    
        void add_force(glm::vec3 force, float delta_time)
        {
			acceleration += force * delta_time;
        }
};



#endif