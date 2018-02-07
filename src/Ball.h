#ifndef BALL_H
#define BALL_H

#include "RigidObject.h"
#include "Shader.h"

class Ball : public RigidObject
{
	public:
		Model model;

		Ball(std::string n, float mass_) : RigidObject(n, mass_)
		{
			// cleaner not to do anything here and just set model later...
		}

		void draw(Shader shader)
		{/*
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex0);
        	shader.setInt("tex", 0);
*/
			model.draw(shader);
		}

		void to_string()
		{
			printf("Position: %f %f %f\nAcceleration: %f %f %f\nVelocity: %f %f %f\n\n",
					position.x, position.y, position.z, acceleration.x, acceleration.y, acceleration.z, velocity.x, velocity.y, velocity.z);
		}
};








#endif
