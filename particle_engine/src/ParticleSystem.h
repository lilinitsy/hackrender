#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "FireParticle.h"
#include "FireworkParticle.h"
#include "WaterParticle.h"
#include "Shader.h"

/*
	Try a circle first for th shape.
	Use instancing to not draw a million models.
		The particle won't have a model, the particle system will draw
		a model at each particle position.

	mean_particle_num: PER FRAME. Reeves pg. 361
	mean_velocity: each instantiated particle will be mean_velocity + some random variance
	mean:colour: same as above
	rate_of_change_colour: Some PDE??
*/


class ParticleSystem
{
	public:
		vector<Particle*> particles;
		float radius;
		glm::vec3 spawn_position;

		float mean_particle_num;
		float variance_particle_num;
		float rate_of_change_particle_num;

		glm::vec3 mean_velocity;
		glm::vec3 variance_velocity;

		glm::vec3 mean_colour;
		glm::vec3 variance_colour;
		float rate_of_change_colour;
		float alpha;

		glm::vec3 mean_size;
		glm::vec3 variance_size;
		float rate_of_change_size;

		float lifetime;
		float lifetime_variance;

		bool use_sphere;
		Model model;
		std::string type;

		glm::vec3 particlesystem_velocity;
		glm::vec3 particlesystem_acceleration;
		bool moveable;


		ParticleSystem()
		{
			radius = 1.0f;
			spawn_position  = glm::vec3(0.0f, 0.0f, 0.0f);
			mean_particle_num = 10;
			variance_particle_num = 2;
			rate_of_change_particle_num = 1.2f;
			mean_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			variance_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			mean_colour = glm::vec3(.5f, .2f, .2f);
			variance_colour = glm::vec3(0.1f, 0.1f, 0.1f);
			rate_of_change_colour = 1.2f;
			alpha = 1.0f;
			mean_size  = glm::vec3(0.005f, 0.005f, 0.005f);
			variance_size  = glm::vec3(0.0001f, 0.0001f, 0.0001f);
			rate_of_change_size = 0.000001f;
			lifetime = 10.0f;
			lifetime_variance = 10.0f;
			use_sphere = false; // default circle
			model; // have to find a beer model
			type = "beer"; // I don't even like beer
			particlesystem_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			particlesystem_acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
			moveable = false;
		}

		ParticleSystem(float r, glm::vec3 sp,
						float mpn, float vpn, float rocpn, float a,
						glm::vec3 mv, glm::vec3 vv,
						glm::vec3 mc, glm::vec3 vc, float rocc,
						glm::vec3 ms, glm::vec3 vs, float rocs, 
						float l, float lv, Model m, bool us, std::string t,
						glm::vec3 psv, glm::vec3 psa, bool move)
		{
			radius = r;
			spawn_position = sp;
			mean_particle_num = mpn;
			variance_particle_num = vpn;
			rate_of_change_particle_num = rocpn;
			mean_velocity = mv;
			variance_velocity = vv;
			mean_colour = mc;
			variance_colour = vc;
			rate_of_change_colour = rocc;
			alpha = a;
			mean_size = ms;
			variance_size = vs;
			rate_of_change_size = rocs;
			lifetime = l;
			lifetime_variance = lv;
			model = m;	
			use_sphere = us;
			type = t;
			particlesystem_velocity = psv;
			particlesystem_acceleration = psa;
			moveable = move;
		}

		void spawn_particles(float delta_time)
		{
			int num_particles_this_cycle = num_particles_generated_this_cycle(delta_time);

			for(int i = 0; i <= num_particles_this_cycle; i++)
			{
				if(type == "fire")
				{		
					FireParticle *fire_particle = new FireParticle();
					fire_particle->set_particle_position(spawn_position, radius, use_sphere);
					fire_particle->set_particle_size(mean_size, variance_size);
					fire_particle->set_particle_colour(mean_colour, variance_colour, alpha);
					fire_particle->set_particle_velocity(mean_velocity, variance_velocity);
					fire_particle->set_lifetime(lifetime, lifetime_variance);
					particles.push_back(fire_particle);
				}

				else if(type == "water")
				{
					WaterParticle *water_particle = new WaterParticle();
					water_particle->set_particle_position(spawn_position, radius, use_sphere);
					water_particle->set_particle_size(mean_size, variance_size);
					water_particle->set_particle_colour(mean_colour, variance_colour, alpha);
					water_particle->set_particle_velocity(mean_velocity, variance_velocity);
					water_particle->set_lifetime(lifetime, lifetime_variance);
					particles.push_back(water_particle);
				}
			}
		}

		virtual void draw_and_update_particles(Shader shader, float delta_time)
		{
			vector<int> particles_to_kill;
			
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

			if(moveable)
			{
				update_particlesystem_position(delta_time);
			}
		}

		virtual void check_collisions(ParticleSystem other_system)
		{
			std::vector<int> particles_to_kill;

			#pragma omp parallel for // this doesn't improve performance? dubya tuh eff???
			for(int i = 0; i < particles.size(); i++)
			{
				for(int j = 0; j < other_system.particles.size(); j++)
				{
					if(particles.size() > 10.0f && !should_kill_particle(*particles[i], i) && particles[i]->collides_with(other_system.particles[j]))
					{
						particles_to_kill.push_back(i);
					}
				}
			}

			if(particles_to_kill.size() > 0)
			{
				kill_particles(particles_to_kill);
			}

		}

	protected:
		virtual bool should_kill_particle(Particle particle, int index)
		{
			if(particle.time_lived > particle.lifetime)
			{
				return true;
			}

			return false;
		}

		virtual void kill_particles(std::vector<int> particles_to_kill)
		{
			for(int i = 0; i < particles_to_kill.size(); i++)
			{
				particles.erase(particles.begin() + particles_to_kill[i] - i);
			}
		}

		

		virtual int num_particles_generated_this_cycle(float delta_time)
		{
			float num_particles_real = (mean_particle_num + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_particle_num))) * delta_time;
			int num_particles_int = num_particles_real * 1000 - (int) num_particles_real * 1000;
			int chance_for_portion = rand() % 1000 + 1;

			if(chance_for_portion >= num_particles_int)
			{
				return (int) num_particles_real + 1.0f;
			}

			return (int) num_particles_real;
		}

		virtual void update_particlesystem_position(float delta_time)
		{
			particlesystem_velocity += particlesystem_acceleration * delta_time;
			spawn_position += particlesystem_velocity * delta_time;			
		}

		
};

#endif
