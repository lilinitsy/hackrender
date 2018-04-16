#ifndef AGENT_H
#define AGENT_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "GameObject.h"
#include "Graph.h"
#include "model.h"
#include "Octree.h"
#include "Obstacle.h"
#include "raycast.h"
#include "Shader.h"
#include "utils.h"

/*
AGENT:
	Each agent stores a few things about itself, as well as (for now) a copy of the graph, path, goal, etc.
	TODO: need to write the to_string() function
			Should probably change its search to DStarLiteSearch when that's worked on.

*/


class Agent : public GameObject
{
	public:
		glm::vec3 velocity; // clamp to have length 5
		Model model;
		float vision_distance = 20.0f;
		// INHERITS glm::vec3 position FROM GameObject in GameObject.h
		// INHERITS float radius FROM GameObject in GameObject.h


		Agent()
		{
			position = glm::vec3(0.0f, 0.0f, 0.0f);
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			can_move = true;
			radius = 0.1f;
		}

		Agent(glm::vec3 p, float rad)
		{
			position = p;
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			can_move = true;
			radius = rad;
		}

		void draw(Shader shader)
		{
			glm::mat4 model_mat4 = setModel(position, glm::vec3(0.005f, 0.005f, 0.005f), 3.14f, glm::vec3(1.0f, 1.0f, 1.0f));
			shader.setMat4("model", model_mat4);
			shader.setVec4("inColour", glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

			model.draw(shader);
		}


		/* 	void update
		 *	params: float delta_time takes in a float that is the change in time
		 * 	function: 
		 * 
		*/
		void update(float delta_time)
		{
			if(glm::length(velocity) > 1.0f)
			{
				velocity = glm::normalize(velocity);
			}
			position += velocity * delta_time;
		}

		void to_string()
		{
			// TODO
		}

	private:	
		bool can_see(GraphNode *node, std::vector<Obstacle*> obstacle_list)
		{
			glm::vec3 ray_dir(node->position - position);
			ray_dir = glm::normalize(ray_dir);
			Ray ray = Ray(position, ray_dir);

			for(int i = 0; i < obstacle_list.size(); i++)
			{						
				if(intersection_occurs(ray, obstacle_list[i], radius))
				{
					return false;
				}
			}

			return true;
		}
};

bool operator==(Agent a1, Agent a2)
{
	if(a1.position == a2.position && 
		a1.velocity == a2.velocity)
	{
		return true;
	}

	return false;
}

bool operator!=(Agent a1, Agent a2)
{
	if(a1.position == a2.position && 
		a1.velocity == a2.velocity)
	{
		return false;
	}

	return true;
}


#endif