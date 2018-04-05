#ifndef RIGIDOBJECT_H
#define RIGIDOBJECT_H

#include "GameObject.h"

class RigidObject : public GameObject
{
	public:
		float mass;

		glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

		RigidObject()
		{
			mass = 1.0f;
		}

		RigidObject(std::string n, float mass_) : GameObject(n)
		{
			mass = mass_;
		}

		void add_force(glm::vec3 force, float delta_time)
		{
			acceleration += (force / mass) * delta_time;
		}

		void update_velocity(float delta_time)
		{
			velocity += acceleration * delta_time;
			position += velocity * delta_time + 0.5f * acceleration * powf(delta_time, 2.0f);
		}

		// should probably have some (private?) collision func
};

#endif
