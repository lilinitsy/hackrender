#ifndef QUEUE_H
#define QUEUE_H

#include <tuple>
#include <vector>

#include "Graph.h"

struct GraphQueue
{
	std::vector<std::tuple<GraphNode*, GraphNode*, float, float>> nodes;

	GraphQueue()
	{

	}

	void push(std::tuple<GraphNode*, GraphNode*, float, float> n)
	{
		nodes.insert(nodes.end(), n);
	}

	void push(GraphNode *node, GraphNode *parent, float g)
	{
		float f = node->heuristic + g;
		nodes.insert(nodes.end(), std::make_tuple(node, parent, g, f));
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

struct PriorityQueue
{
	/* 	
		First:	node to explore
		second: parent
		third:	gscore - distance from parent to child
		fourth: fscore - g + node->heuristic
	*/
	std::vector<std::tuple<GraphNode*, GraphNode*, float, float>> nodes;

	PriorityQueue()
	{

	}

	void push(GraphNode *node, GraphNode *parent, float g)
	{
		float f = node->heuristic + g;

		for(int i = 0; i < nodes.size(); i++)
		{
			if(f < std::get<3>(nodes[i]))
			{
				nodes.insert(nodes.begin() + i, std::make_tuple(node, parent, g, f));
				return;
			}
		}

		nodes.insert(nodes.end(), std::make_tuple(node, parent, g, f));
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


#endif