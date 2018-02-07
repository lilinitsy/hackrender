#ifndef PARTICLE_H
#define PARTICLE_H

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "model.h"


class Particle
{
	public:
		glm::vec3 position;
		glm::vec3 size;
		glm::vec3 colour;
		glm::vec3 transparency;
		glm::vec3 velocity;

		float lifetime = INFINITY;
		float time_lived = 0.0f;
		float alpha;

		Particle()
		{
			position = glm::vec3(10.0f, 10.0f, 10.0f);
			size = glm::vec3(1.0f, 1.0f, 1.0f);
			colour = glm::vec3(1.0f, 1.0f, 1.0f);
			transparency = glm::vec3(1.0f, 1.0f, 1.0f);
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			lifetime = INFINITY;
			alpha = 1.0f;
		}


		Particle(glm::vec3 p, glm::vec3 s, glm::vec3 c, glm::vec3 t, glm::vec3 v, float l, float a)
		{
			position = p;
			size = s;
			colour = c;
			transparency = t;
			velocity = v;
			lifetime = l;
			alpha = a;
		}


		void draw(Model model, Shader shader)
		{
			glm::mat4 model2 = setModel(position, size, 3.14f, glm::vec3(1.0f, 1.0f, 1.0f));
			shader.setMat4("model", model2);
			shader.setVec4("inColour", glm::vec4(colour, alpha));
			glEnable(GL_BLEND);
        	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			model.draw(shader);
		}


		void to_string()
		{
			printf("Position: %f %f %f\n", position.x, position.y, position.z);
			printf("Size: %f %f %f\n", size.x, size.y, size.z);
			printf("Colour: %f %f %f\n", colour.x, colour.y, colour.z);
			printf("Velocity: %f %f %f\n", velocity.x, velocity.y, velocity.z);
		}


		virtual void update_particle(float delta_time)
		{
			position += velocity * delta_time;
			time_lived += delta_time;
		}


		virtual void set_particle_position(glm::vec3 spawn_position, float radius, bool use_sphere)	
		{
			float pos_x = spawn_position.x;
			float pos_y = spawn_position.y;
			float pos_z = spawn_position.z;

			if(!use_sphere)
			{
				pos_x = spawn_position.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / radius));
				pos_y = spawn_position.y;					
				pos_z = spawn_position.z + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / radius));
			}

			else
			{
				// TODO
			}

			position = glm::vec3(pos_x, pos_y, pos_z);
		}


		virtual void set_particle_velocity(glm::vec3 mean_velocity, glm::vec3 variance_velocity)
		{
			float velocity_x = mean_velocity.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_velocity.x));
			float velocity_y = mean_velocity.y + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_velocity.y));
			float velocity_z = mean_velocity.z + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_velocity.z));

			velocity = glm::vec3(velocity_x, velocity_y, velocity_z);
		}


		virtual void set_particle_colour(glm::vec3 mean_colour, glm::vec3 variance_colour, float a)
		{
			float colour_x = mean_colour.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_colour.x));
			float colour_y = mean_colour.y + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_colour.y));
			float colour_z = mean_colour.z + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_colour.z));
			alpha = a;

			colour = glm::vec3(colour_x, colour_y, colour_z);

		}


		virtual void set_particle_size(glm::vec3 mean_size, glm::vec3 variance_size)
		{
			float size_x = mean_size.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_size.x));
			float size_y = mean_size.y + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_size.y));
			float size_z = mean_size.z + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / variance_size.z));

			size = glm::vec3(size_x, size_y, size_z);
		}


		virtual void set_lifetime(float l, float lv)
		{
			lifetime = l + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / lv));
		}

		bool collides_with(Particle *op)
		{

			if(op->position.x + op->size.x > position.x &&
				op->position.x < position.x + size.x &&
				op->position.y + op->size.y > position.y &&
				op->position.y < position.y + size.y && 
				op->position.z + op->size.z > position.z &&
				op->position.z < position.z + size.z)
			{
				return true;
			}

			return false;
		}
};

#endif
