#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"


struct GraphNode
{
	glm::vec3 position;
	std::vector<GraphNode*> neighbours;
	std::vector<float> costs;
	float heuristic;
	bool visited = false;
	bool is_goal = false;

	GraphNode()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		heuristic = 1.0f;
	}

	GraphNode(glm::vec3 p, float h)
	{
		position = p;
		heuristic = h;
	}

	void to_string()
	{
		printf("Node position: %f %f %f\n", position.x, position.y, position.z);
		printf("Heuristic: %f\n", heuristic);
		
		/*for(int i = 0; i < neighbours.size(); i++)
		{
			printf("Neighbour %d position: %f %f %f\n", i, neighbours[i]->position.x, neighbours[i]->position.y, neighbours[i]->position.z);
			printf("Neighbour %d cost: %f\n", i, costs[i]);
		}*/
	}
};

#endif