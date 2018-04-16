#ifndef QUEUE_H
#define QUEUE_H

#include <algorithm>
#include <tuple>
#include <vector>

#include "Graph.h"
#include "utils.h"

/*
	For D* Lite:
		First: Node to explore (successor)
		Second: Parent
		Third: g
		Fourth: rhs
		fifth: tuple for keys;
			min(g(x), rhs(x) + h(x)); min(g(x), rhs(s))

*/

struct GraphQueueLPA
{
	std::vector<std::pair<GraphNode*, GraphNode*>> nodes;

	GraphQueueLPA()
	{

	}

	void push(GraphNode *node, GraphNode *parent)
	{

	}

	void push(std::pair<GraphNode*, GraphNode*> n)
	{
		nodes.insert(nodes.end(), n);
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
			if(child == nodes[i].first)
			{
				nodes[i].second = parent;
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


struct PriorityQueueLPA
{
	/* 	
		For A*:
			First:	node to explore
			second: parent
			third:	gscore - distance from parent to child
			fourth: fscore - g + node->heuristic
	
		For D* Lite:
			First: Node to explore (successor)
			Second: Parent
			Third: g
			Fourth: rhs
			fifth: key
	*/
	std::vector<std::pair<GraphNode*, GraphNode*>> nodes;

	PriorityQueueLPA()
	{

	}

	void push(GraphNode *node, GraphNode *parent)
	{
		float key1 = node->key.first;
		float key2 = node->key.second;
		std::pair<float, float> key = std::make_pair(key1, key2);

		for(int i = 0; i < nodes.size(); i++)
		{
			if(key.first < nodes[i].first->key.first ||
			  (key.first == nodes[i].first->key.first && 
			  	key.second < nodes[i].first->key.second))
			{
				nodes.insert(nodes.begin() + i, std::make_pair(node, parent));
				return;
			}
		}

		nodes.insert(nodes.end(), std::make_pair(node, parent));
	}

	void pop()
	{
		nodes.erase(nodes.begin());
	}

	void remove(GraphNode *n)
	{
		for(int i = 0; i < nodes.size(); i++)
		{
			if(n == std::get<0>(nodes[i]))
			{
				nodes.erase(nodes.begin() + i);
			}
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


	/* 	
		For A*:
			First:	node to explore
			second: parent
			third:	gscore - distance from parent to child
			fourth: fscore - g + node->heuristic
	*/

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
		For A*:
			First:	node to explore
			second: parent
			third:	gscore - distance from parent to child
			fourth: fscore - g + node->heuristic
	
		For D* Lite:
			First: Node to explore (successor)
			Second: Parent
			Third: g
			Fourth: rhs
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