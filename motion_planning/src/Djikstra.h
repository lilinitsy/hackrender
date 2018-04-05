#ifndef DJIKSTRA_H
#define DJIKSTRA_H

#include <tuple>
#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Graph.h"
#include "Queue.h"
#include "Search.h"


struct GraphQueueD
{
	std::vector<std::tuple<GraphNode*, GraphNode*, float>> nodes;

	GraphQueueD()
	{

	}

	void push(std::tuple<GraphNode*, GraphNode*, float> n)
	{
		nodes.insert(nodes.end(), n);
	}

	void push(GraphNode *node, GraphNode *parent, float g)
	{
		nodes.insert(nodes.end(), std::make_tuple(node, parent, g));
	}

	void pop()
	{
		nodes.erase(nodes.begin());
	}

	void swap_child(GraphNode *node_in_queue, GraphNode *node_to_be_placed)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(node_in_queue == std::get<0>(nodes[i]))
			{
				std::get<0>(nodes[i]) = node_to_be_placed;
				return;
			}
		}
	}

	void swap_parent_of_child(GraphNode *child, GraphNode *parent)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(child == std::get<0>(nodes[i]))
			{
				std::get<1>(nodes[i]) = parent;
				return;
			}
		}
	}

	void to_string()
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			GraphNode *current = std::get<0>(nodes[i]);
			GraphNode *parent = std::get<1>(nodes[i]);
			printf("vector index %d (position, parent position) (%f, %f, %f), (%f, %f, %f)\n", 
					i, current->position.x, current->position.y, current->position.z,
						parent->position.x, parent->position.y, parent->position.z);
		}
	}

	bool node_in_queue(GraphNode *n)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(n == std::get<0>(nodes[i]))
			{
				return true;
			}
		}

		return false;
	}
};

struct PriorityQueueD
{
	/* 	
		First:	node to explore
		second: parent
		third:	gscore - distance from parent to child
		fourth: fscore - g + node->heuristic
	*/
	std::vector<std::tuple<GraphNode*, GraphNode*, float>> nodes;

	PriorityQueueD()
	{

	}

	void push(GraphNode *node, GraphNode *parent, float g)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(g < std::get<2>(nodes[i]))
			{
				nodes.insert(nodes.begin() + i, std::make_tuple(node, parent, g));
				return;
			}
		}

		nodes.insert(nodes.end(), std::make_tuple(node, parent, g));
	}

	void pop()
	{
		nodes.erase(nodes.begin());
	}

	bool node_in_queue(GraphNode *n)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(n == std::get<0>(nodes[i]))
			{
				return true;
			}
		}

		return false;
	}
};


class DjikstraSearch : public Search
{
	public:
		DjikstraSearch()
		{

		}

		std::vector<GraphNode*> find_path(std::vector<GraphNode*> graph, GraphNode *start, GraphNode *goal)
		{
			PriorityQueueD fringe;
			GraphQueueD explored;
			GraphQueueD path;

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
					
					// if the neighbour has been put in explored
					// not sure if this block will need to be removed.
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
		std::vector<GraphNode*> reconstruct_graph(GraphQueueD path)
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