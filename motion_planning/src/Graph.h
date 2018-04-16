#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"


/*
	GraphNode struct stores the position of each node, each of its neighbours, and the costs.
		Also has a heurustic, whether this node is the goal, and whether it has been visited.
		The bool visited is probably not necessary.
*/


struct GraphNode
{
	glm::vec3 position;
	std::vector<GraphNode*> neighbours;
	std::vector<float> costs;
	float heuristic;
	bool visited = false;
	bool is_goal = false;

	float g = INFINITY;
	float rhs = INFINITY;
	std::pair<float, float> key;

	GraphNode()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		heuristic = 1.0f;
		key  = std::make_pair(
							std::min(g, rhs + heuristic),
							std::min(g, rhs));
	}

	GraphNode(glm::vec3 p, float h)
	{
		position = p;
		heuristic = h;
		key  = std::make_pair(
							std::min(g, rhs + heuristic),
							std::min(g, rhs));
	}

	void calculate_key()
	{
		key = std::make_pair(
							std::min(g, rhs + heuristic),
							std::min(g, rhs));
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