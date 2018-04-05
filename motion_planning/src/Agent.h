#ifndef AGENT_H
#define AGENT_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "AStarSearch.h"
#include "Djikstra.h"
#include "GameObject.h"
#include "Graph.h"
#include "model.h"
#include "Octree.h"
#include "Obstacle.h"
#include "raycast.h"
#include "Shader.h"
#include "utils.h"


class Agent : public GameObject
{
	public:
		glm::vec3 velocity; // clamp to have length 5
		Model model;
		AStarSearch search;
		float vision_distance = 20.0f;
		// INHERITS glm::vec3 position FROM GameObject in GameObject.h
		// INHERITS float radius FROM GameObject in GameObject.h

		std::vector<GraphNode*> graph;
		std::vector<Model> node_models;
		std::vector<GraphNode*> path;
		GraphNode *goal;


		Agent()
		{
			position = glm::vec3(0.0f, 0.0f, 0.0f);
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			Search();
			can_move = true;
			radius = 0.1f;
		}

		Agent(glm::vec3 p, float rad)
		{
			position = p;
			velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			Search();
			can_move = true;
			radius = rad;
		}

		/*
		COMMENTING OUT FOR NOW, WILL LIKELY BRING THIS BACK IN SOME FORM WHEN DOING A D* VARIANT

		void construct_graph(int num_nodes, glm::vec3 goal_position, Octree obstacle_octree, std::vector<Obstacle*> obstacle_list)
		{
			for(int i = 0; i < num_nodes; i++)
			{
				float x = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 30.0f));
				float y = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
				float z = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 40.0f));
				bool point_is_feasible = true;

				glm::vec3 node_position(x, y, z);
				glm::vec3 ray_dir(position - node_position);
				ray_dir = glm::normalize(ray_dir);
				Ray ray = Ray(position, ray_dir);

				// need to test against the octree depth / node 
				// near the node_position.

				Octree obstacle_octree_copy = obstacle_octree;
				while(obstacle_octree_copy != obstacle_octree.octree_containing_point(node_position))
				{
					obstacle_octree_copy = obstacle_octree_copy.octree_containing_point(node_position);
				}

				for(int i = 0; i < obstacle_octree_copy.objects.size(); i++)
				{
					if(intersection_occurs(ray, obstacle_octree_copy.objects[i], radius))
					{
						point_is_feasible = false;
					}
				}

				if(point_is_feasible)
				{
					float heuristic = glm::length(goal_position - node_position);
					GraphNode *node = new GraphNode(node_position, heuristic);
					graph.push_back(node);
				}

				else
				{
					i--;
				}
			}
	
			GraphNode *start_node = new GraphNode(position, glm::length(position - goal_position));
			GraphNode *goal_node = new GraphNode(goal_position, 0.0f);
			goal_node->is_goal = true;
			graph.push_back(start_node);
			graph.push_back(goal_node);
			goal = goal_node;
			
			make_graph_neighbours(obstacle_list, obstacle_octree, goal_position);
			visualize_node_positions();
			path = search.find_path(graph, start_node, goal_node);
			printf("*******\nDONE WITH AGENT\n**********\n");
		}
		*/
		void draw(Shader shader)
		{
			glm::mat4 model_mat4 = setModel(position, glm::vec3(0.005f, 0.005f, 0.005f), 3.14f, glm::vec3(1.0f, 1.0f, 1.0f));
			shader.setMat4("model", model_mat4);
			shader.setVec4("inColour", glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));

			model.draw(shader);
		}

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
			for(int i = 0; i < graph.size(); i++)
			{
				printf("*************\n");
				graph[i]->to_string();
			}
		}

	private:	
		/*
		void make_graph_neighbours(std::vector<Obstacle*> obstacle_list, Octree obstacle_octree, glm::vec3 goal_position)
		{
			for(int i = 0; i < graph.size(); i++)
			{
				glm::vec3 node_position(graph[i]->position.x, graph[i]->position.y, graph[i]->position.z);

				for(int j = 0; j < graph.size(); j++)
				{
					if(graph[i] != graph[j])
					{
						glm::vec3 ray_dir(graph[j]->position - graph[i]->position);
						ray_dir = glm::normalize(ray_dir);
						Ray ray = Ray(position, ray_dir);
						bool node_j_is_neighbour = true;
					
						for(int i = 0; i < obstacle_list.size(); i++)
						{						
							if(intersection_occurs(ray, obstacle_list[i], radius))
							{
								node_j_is_neighbour = false;
								break;
							}
						} 

						if(node_j_is_neighbour)
						{
							graph[i]->neighbours.push_back(graph[j]);
							graph[i]->costs.push_back(glm::length(graph[i]->position - graph[j]->position));
						}
					}
				}

				// now test if the goal is a neighbour

			}
		}

		void visualize_node_positions()
		{
			for(int i = 0; i < graph.size(); i++)
			{
				Model model = Model("../models/cube/cube.obj");
				node_models.push_back(model);
			}
		}

		void avoid_collision_with_agents(std::vector<Obstacle*> obstacle_list, std::vector<Agent*> agent_list, float delta_time)
		{

		}
		
		void goal_force()
		{
			
		}

		void nearest_node_on_path_force()
		{

		}

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
		*/
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