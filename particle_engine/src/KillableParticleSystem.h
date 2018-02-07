#ifndef KILLABLEPARTICLESYSTEM_H
#define KILLABLEPARTICLESYSTEM_H

#include "ParticleSystem.h"
#include "FireworkParticle.h"

class KillableParticleSystem : public ParticleSystem
{
    public:
		float system_lifetime;
		float time_lived = 0.0f;
		bool system_killed = false;
		Particle launching_particle;

        KillableParticleSystem() : ParticleSystem()
        {
			launching_particle = Particle();
			system_lifetime = 10.0f;
        }
    
        KillableParticleSystem(float r, glm::vec3 sp,
						float mpn, float vpn, float rocpn, float a,
						glm::vec3 mv, glm::vec3 vv,
						glm::vec3 mc, glm::vec3 vc, float rocc,
						glm::vec3 ms, glm::vec3 vs, float rocs, 
						float l, float lv, Model m, bool us, std::string t,
						glm::vec3 psv, glm::vec3 psa, bool move, float psl) : 
            ParticleSystem(r, sp, mpn, vpn, rocpn, a, mv, vv, mc, vc, rocc,
                            ms, vs, rocs, l, lv, m, us, t, psv, psa , move)
        {
			launching_particle = Particle(spawn_position, mean_size * 5.0f, mean_colour, glm::vec3(1.0f, 1.0f, 1.0f), mean_velocity, system_lifetime, a);
			system_lifetime = psl;
        }
 
		void update_system(Shader shader, float delta_time)
		{
			time_lived += delta_time;

			if(time_lived >= system_lifetime && !system_killed)
			{
				system_killed = true;
				on_death();
			}

			else if(time_lived < system_lifetime && !system_killed)
			{
				launching_particle.position = spawn_position;
				launching_particle.draw(model, shader);
				update_particlesystem_position(delta_time);
			}
		}

		void draw_and_update_particles(Shader shader, float delta_time)
		{
			vector<int> particles_to_kill;

			if(particles.size() > 0)
			{
				// #pragma omp parallel for --> it isn't improving at all??
				for(int i = 0; i < particles.size(); i++)
				{
					if(!should_kill_particle(*particles[i], i))
					{
						particles[i]->draw(model, shader);
						particles[i]->update_particle(delta_time);
					}

					else
					{
						particles_to_kill.push_back(i);
					}
				}

				if(particles_to_kill.size() > 0)
				{
					kill_particles(particles_to_kill);
				}
			}
		}


	private:
		void on_death()
		{
			int num_firework_particles_to_spawn = (mean_particle_num - variance_particle_num) + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (mean_particle_num + variance_particle_num)));

			for(int i = 0; i < num_firework_particles_to_spawn; i++)
			{
				FireworkParticle *firework_particle = new FireworkParticle();
				firework_particle->set_particle_position(spawn_position, radius, use_sphere);
				firework_particle->set_particle_size(mean_size / 2.0f, variance_size / 2.0f);
				firework_particle->set_particle_colour(alpha);
				firework_particle->set_particle_velocity(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(3.0f, 3.0f, 3.0f));
				firework_particle->set_lifetime(lifetime, lifetime_variance);
				particles.push_back(firework_particle);
			}
		}


};



#endif