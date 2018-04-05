#ifndef ASTARSEARCH_H
#define ASTARSEARCH_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Graph.h"
#include "Queue.h"
#include "Search.h"

class AStarSearch : public Search
{
	public:
		AStarSearch()
		{

		}

		std::vector<GraphNode*> find_path(std::vector<GraphNode*> graph, GraphNode *start, GraphNode *goal)
		{
			PriorityQueue fringe;
			GraphQueue explored;
			GraphQueue path;

			fringe.push(start, start, 0.0f);

			while(fringe.nodes.size() > 0)
			{
				GraphNode *current = std::get<0>(fringe.nodes[0]);
				float current_g = std::get<2>(fringe.nodes[0]);
				if(current == goal)
				{
					printf("PATH TO GOAL PLANNED\n");
					GraphNode *parent = std::get<1>(fringe.nodes[0]);
					path.push(current, parent, glm::length(parent->position - current->position));
					return reconstruct_graph(path);
				}

				explored.push(fringe.nodes[0]);
				path.push(fringe.nodes[0]);
				fringe.pop();

				for(int i = 0; i < current->neighbours.size(); i++)
				{
					float g = glm::length(current->position - current->neighbours[i]->position) + current_g;
					
					if(explored.node_in_queue(current->neighbours[i]))
					{
						continue;
					}

					if(!fringe.node_in_queue(current->neighbours[i]))
					{
						fringe.push(current->neighbours[i], current, g);
					}

					if(g < current_g)
					{
						if(path.node_in_queue(current->neighbours[i]))
						{
							path.swap_parent_of_child(current->neighbours[i], current);
						}

						else
						{
							path.push(current->neighbours[i], current, g);
						}
					}
				}
			}

			printf("Full path not found, returning what was\n");
			return reconstruct_graph(path);
		}

	private:
		std::vector<GraphNode*> reconstruct_graph(GraphQueue path)
		{
			std::vector<GraphNode*> final_path;
			path.to_string();

			for(int i = 0; i < path.nodes.size(); i++)
			{
				final_path.push_back(std::get<0>(path.nodes[i]));
			}

			return final_path;
		}
};

#endif