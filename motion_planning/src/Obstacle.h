#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "GameObject.h"
#include "model.h"
#include "Shader.h"
#include "utils.h"

class Obstacle : public GameObject
{
	public:
		glm::vec3 velocity;
		Model model;
		// INHERITS glm::vec3 position FROM GameObject in GameObject.h
		// INHERITS float radius FROM GameObject in GameObject.h
		
		Obstacle()
		{
			position = glm::vec3(0.0f, 0.0f, 0.0f);
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			can_move = false;
			radius = 0.5f;
		}

		Obstacle(glm::vec3 p, float rad)
		{
			position = p;
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			can_move = false;
			radius = rad;
		}

		Obstacle(glm::vec3 p, glm::vec3 v, float rad)
		{
			position = p;
			velocity = v;
			can_move = true;
			radius = rad;
		}

		void draw(Shader shader)
		{
			glm::mat4 model_mat4 = setModel(position, glm::vec3(0.005f, 0.005f, 0.005f), 3.14f, glm::vec3(1.0f, 1.0f, 1.0f));
			shader.setMat4("model", model_mat4);
			shader.setVec4("inColour", glm::vec4(0.5f, 1.0f, 0.5f, 1.0f));

			model.draw(shader);
		}
};

#endif