#ifndef DSTARLITESEARCH_H
#define DSTARLITESEARCH_H


#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Graph.h"
#include "Queue.h"

/* 
NOTATION
	n: node/state
	c(n1, n2) - length of edge connecting
	b(n1) = n2 - back pointer from n1 to n2
	O - Open List stores nodes for expansion, maybe also called Q
	C - Closed list stores nodes explored

	Focused D*:
		focusing heuristic g(X, R) estimates path cost from robot R to location X

	


	D* Lite:
		g: Objective function value. I suppose, euclidean distance?
		rhs: One step lookahead of the objective function value.
		If g != rhs, the node is inconsistent and goes on the PQ fringe.

		rhs calculation:
			let S be the set of all successors of u
			let s be some S
			rhs = minimum(c(u, s) + g(s))




	MORE NOTATION
		x: node
		x': parent of node
		Inconsistent: g(x) < rhs(x)
			Path to that node was made to be more expensive; ie, an obstacle
			Put on fringe?

		Overconsistent: g(x) > rhs(x)
			Path was less expensive. Implies shortest path to that node was found.

		rhs: Cost to the parent + cost of travel to the node
			rhs(x) = min(g(x') + c(x', x))
				This means the rhs of the current node is the 
				minimum of any of it's parents g(x) + the cost from the parent to the node.
				So, have to compare the cost of each parent's g(x) + their cost to the node for 
				each node?
						
*/

class DStarLiteSearch
{
	public:
		GraphQueueLPA path;
		GraphQueueLPA graph;
		GraphQueueLPA explored;
		PriorityQueueLPA fringe;

		DStarLiteSearch()
		{

		}

		std::vector<GraphNode*> update(std::vector<GraphNode*> nodes, GraphNode *start, GraphNode *goal)
		{
			// make sure to call initalize before update, the first time at least
			printf("got into update\n");
			compute_shortest_path(nodes, start, goal);
			for(int i = 0; i < path.nodes.size(); i++)
			{
				
			}
			return reconstruct_graph();
		}

		void compute_shortest_path(std::vector<GraphNode*> nodes, GraphNode *start, GraphNode *goal)
		{
			goal->calculate_key();
			while(fringe.nodes[0].first->key < goal->key || goal->rhs != goal->g)
			{
				GraphNode *current = fringe.nodes[0].first;
				path.push(fringe.nodes[0]);
				fringe.pop();

				if(current->g > current->rhs) // overconsistent
				{
					current->g = current->rhs; // makes this consistent
				}

				else
				{
					current->g = INFINITY; // makes this overconsistent or consistent
				}
				printf("Got through compute_shortest_path\n");

				for(int i = 0 ; i < current->neighbours.size(); i++)
				{
					update_vertex(current->neighbours[i], current, start);
				}
			}
		}

		void update_vertex(GraphNode *node, GraphNode *parent, GraphNode *start)
		{
			if(node != start)
			{
				float min_rhs = INFINITY;
				for(int i = 0; i < node->neighbours.size(); i++)
				{
					float tmp_rhs = node->neighbours[i]->g + glm::length(node->position - node->neighbours[i]->position);
					if(tmp_rhs < min_rhs)
					{
						min_rhs = tmp_rhs;
					}
				}
				node->rhs = min_rhs;
			}

			if(fringe.node_in_queue(node))
			{
				fringe.remove(node);
			}

			if(node->g != node->rhs)
			{
				node->calculate_key();
				fringe.push(node, parent);
			}
		}

		void initialize(std::vector<GraphNode*> nodes, GraphNode *start, GraphNode *goal)
		{
			printf("Size of graph: %lu\n", nodes.size());
			for(int i = 0; i < nodes.size(); i++)
			{
				nodes[i]->g = INFINITY;
				nodes[i]->rhs = INFINITY;
				printf("i: %d\n", i);
			}

			start->rhs = 0;
			start->calculate_key();
			printf("Start key calculated\n");
			fringe.push(start, start);
			path.push(start, start);
		}

	private:
		std::vector<GraphNode*> reconstruct_graph()
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