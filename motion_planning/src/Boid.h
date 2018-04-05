#ifndef BOID_H
#define BOID_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Agent.h"
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

class Boid
{
	public:
		std::vector<Agent*> agents;
		std::vector<Obstacle*> obstacles;
		std::vector<GraphNode*> graph;
		std::vector<Model> node_models;
		std::vector<GraphNode*> path;
		GraphNode *goal;
		AStarSearch search;
		float k = 30.0f;
		float mass = 4.0f;
		float max_velocity = 1.0f;

		glm::vec3 average_position = glm::vec3(0.0f, 0.0f, 0.0f);

		Boid()
		{
			Search();
		}


		void construct_graph(int num_nodes, glm::vec3 goal_position, Octree obstacle_octree)
		{
			average_position = glm::vec3(0.0f, 0.0f, 0.0f);
			for(int i = 0; i < agents.size(); i++)
			{
				average_position += agents[i]->position;
			}		

			average_position /= agents.size();

			for(int i = 0; i < num_nodes; i++)
			{
				float x = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 30.0f));
				float y = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
				float z = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 40.0f));
				bool point_is_feasible = true;

				glm::vec3 node_position(x, y, z);
				glm::vec3 ray_dir(average_position - node_position);
				ray_dir = glm::normalize(ray_dir);
				Ray ray = Ray(average_position, ray_dir);

				Octree obstacle_octree_copy = obstacle_octree;
				while(obstacle_octree_copy != obstacle_octree.octree_containing_point(node_position))
				{
					obstacle_octree_copy = obstacle_octree_copy.octree_containing_point(node_position);
				}

				for(int i = 0; i < obstacle_octree_copy.objects.size(); i++)
				{
					if(intersection_occurs(ray, obstacle_octree_copy.objects[i], agents[0]->radius))
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
	
			GraphNode *start_node = new GraphNode(average_position, glm::length(average_position - goal_position));
			GraphNode *goal_node = new GraphNode(goal_position, 0.0f);
			goal_node->is_goal = true;
			graph.push_back(start_node);
			graph.push_back(goal_node);
			goal = goal_node;
			
			make_graph_neighbours(obstacle_octree, goal_position);
			visualize_node_positions();
			path = search.find_path(graph, start_node, goal_node);
			printf("*******\nDONE WITH AGENT\n**********\n");
		}

		void draw(Shader shader)
		{
			for(int i = 0; i < agents.size(); i++)
			{
				agents[i]->draw(shader);
			}

			for(int i = 0; i < obstacles.size(); i++)
			{
				obstacles[i]->draw(shader);
			}
			
			for(int i = 0; i < node_models.size(); i++)
			{
				glm::mat4 node_model_mat4 = setModel(graph[i]->position, glm::vec3(0.1f, 0.1f, 0.1f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				shader.setMat4("model", node_model_mat4);
				
				if(graph[i]->is_goal)
				{	
					shader.setVec4("inColour", glm::vec4(0.0f, 0.6f, 1.0f, 1.0f));
				}

				else if(graph[i]->visited)
				{
					shader.setVec4("inColour", glm::vec4(1.0f, 1.0f, 0.6f, 1.0f));
				}
				
				else
				{
					shader.setVec4("inColour", glm::vec4(0.0f, 1.0f, 0.6f, 1.0f));
				}

				node_models[i].draw(shader);
			} 
		}

		void update(Octree obstacle_octree, Octree agent_octree, float delta_time)
		{
			if(path.size() == 0 && goal->visited)
			{
				for(int i = 0; i < agents.size(); i++)
				{
					agents[i]->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				}
				printf("AGENT REACHED GOAL\n");	
			}

			else
			{
				for(int i = 0; i < agents.size(); i++)
				{
					if(glm::length(agents[i]->position - path[0]->position) <= 0.1f)
					{
						path[0]->visited = true;
						printf("*****\nReached node on path, removing (position)\n(%f, %f, %f), \n*****\n", 
							path[0]->position.x, path[0]->position.y, path[0]->position.z);
						path.erase(path.begin());


						printf("Path now: \n");
						for(int i = 0; i < path.size(); i++)
						{
							printf("vector index %d: (%f, %f, %f)\n",
								path[i]->position.x, path[i]->position.y, path[i]->position.z);
						}

					}
				}

				calculate_separation_force(delta_time);
				calculate_node_force(delta_time);
				calculate_cohesion_force(delta_time);
				calculate_obstacle_force(delta_time);

				if(path.size() > 1 && can_see(path[1], obstacles))
				{
					printf("*****\CAN SEE NEXT NODE, removing (position)\n(%f, %f, %f), \n*****\n", 
						path[0]->position.x, path[0]->position.y, path[0]->position.z);
					path.erase(path.begin());

					printf("Path now: \n");
					for(int i = 0; i < path.size(); i++)
					{
						printf("vector index %d: (%f, %f, %f)\n",
							path[i]->position.x, path[i]->position.y, path[i]->position.z);
					}
				}

				for(int i  = 0; i < agents.size(); i++)
				{
					agents[i]->update(delta_time);
				}
			}
		}

	private:
		void make_graph_neighbours(Octree obstacle_octree, glm::vec3 goal_position)
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
						Ray ray = Ray(average_position, ray_dir);
						bool node_j_is_neighbour = true;
					
						for(int i = 0; i < obstacles.size(); i++)
						{						
							if(intersection_occurs(ray, obstacles[i], agents[0]->radius))
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

		bool can_see(GraphNode *node, std::vector<Obstacle*> obstacle_list)
		{
			glm::vec3 ray_dir(node->position - agents[0]->position);
			ray_dir = glm::normalize(ray_dir);
			Ray ray = Ray(agents[0]->position, ray_dir);

			for(int i = 0; i < obstacle_list.size(); i++)
			{						
				if(intersection_occurs(ray, obstacle_list[i], agents[0]->radius))
				{
					return false;
				}
			}

			return true;	
		}


		void calculate_separation_force(float delta_time)
		{
			#pragma omp parallel for
			for(int i = 0; i < agents.size(); i++)
			{
				for(int j = 0; j < agents.size(); j++)
				{
					if(agents[i] != agents[j])
					{
						float dist_between_agents = glm::length(agents[i]->position - agents[j]->position - agents[i]->radius - agents[j]->radius);

						if(dist_between_agents <= 2.0f)
						{
							float force = -1.0f * k * (1.0f / dist_between_agents);
							glm::vec3 unit_vec_i = glm::normalize(agents[i]->position - average_position);
							glm::vec3 unit_vec_j = glm::normalize(agents[j]->position - average_position);
							glm::vec3 tmp_vel = 0.1f * (force / mass) * unit_vec_i * (delta_time * delta_time);
							glm::vec3 tmp_vel2 = 0.1f * (force / mass) * unit_vec_j * (delta_time * delta_time);

							agents[i]->velocity += tmp_vel2;
							agents[j]->velocity -= tmp_vel;			
						}
					}
				}
			}
		}

		void calculate_cohesion_force(float delta_time)
		{
			#pragma omp parallel for
			for(int i = 0; i < agents.size(); i++)
			{
				float dist_from_agent_to_average_position = glm::length(average_position - agents[i]->position - agents[i]->radius);
				
				if(dist_from_agent_to_average_position > 0.4f)
				{
					float force = 1.0f * k * (1.0f / (dist_from_agent_to_average_position - agents[i]->radius));
					glm::vec3 unit_vec_i = glm::normalize(agents[i]->position - average_position);
					agents[i]->velocity += 0.5f * (force / mass) * unit_vec_i * (delta_time * delta_time);
				}
			}
		}

		void calculate_node_force(float delta_time)
		{
			//glm::vec3 unit_vec_i = glm::normalize(path[0]->position - average_position); doesn't work??

			#pragma omp parallel for
			for(int i = 0; i < agents.size(); i++)
			{
				float dist_from_agents_to_next_node = glm::length(agents[i]->position - path[0]->position);
				float force = 1.0f * k * (1.0f / dist_from_agents_to_next_node);
				// USING THE AVERAGE POSITION BREAKS EVERYTHING
				glm::vec3 unit_vec_i = glm::normalize(path[0]->position - agents[i]->position);
				agents[i]->velocity += 0.5f * (force / mass) * unit_vec_i * delta_time;
			}
		}

		void calculate_obstacle_force(float delta_time)
		{
			for(int i = 0; i < agents.size(); i++)
			{
				for(int j = 0; j < obstacles.size(); j++)
				{
					if(glm::length(agents[i]->position - obstacles[j]->position) <= 2.0f)
					{
						float dist_from_agent_to_obstacle = glm::length(agents[i]->position - obstacles[j]->position - obstacles[j]->radius - agents[i]->radius);
						float force = -1.0f * k * (dist_from_agent_to_obstacle * dist_from_agent_to_obstacle);
						glm::vec3 unit_vec_i = glm::normalize(obstacles[i]->position - agents[i]->position);
						agents[i]->velocity += 1.0f * (force / mass) * unit_vec_i * (delta_time * delta_time);
					}
				}
			}
		}
};






#endif