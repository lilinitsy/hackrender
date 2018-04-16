#ifndef RAYCAST_H
#define RAYCAST_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "GameObject.h"
#include "ray.h"


/*
	This is a collection of assorted functions used in raycasting.
		ONLY SUPPORTS SPHERES - NEED TO UPDATE FOR OTHER OBJECTS if we want those. 
		Won't be hard to get rectangles in?
*/


// quadratic formula solver. Returning INFINITY means the ray didn't intersect with anything.
float smallest_root(float a, float b, float c)
{
	float discriminant = b * b - 4 * a * c;

	if(discriminant < 0)
	{
		return INFINITY;
	}

	float t1 = ((-b) + sqrt(discriminant)) / (2 * a);
	float t2 = ((-b) - sqrt(discriminant)) / (2 * a);

	if(t1 < t2 && t1 >= 0)
	{
		return t1;
	}

	else if(t2 >= 0)
	{
		return t2;
	}

	return INFINITY;
}

/*
	This constructs the a, b, c parameters in the quadratic formula.
	e_c is a vector from the ray position to the object position.
	
	This is for spheres.
*/
float collision_distance(Ray ray, GameObject *game_object, float agent_radius)
{
	glm::vec3 e_c = ray.position - (game_object->position);// DO NOT DO: + agent_radius);
	float a = glm::dot(ray.direction, ray.direction);
	float b = 2 * glm::dot(ray.direction, e_c);
	float c = glm::dot(e_c, e_c) - (game_object->radius * game_object->radius + agent_radius * agent_radius);

	return smallest_root(a, b, c);
}

bool intersection_occurs(Ray ray, GameObject *game_object, float agent_radius)
{
	float distance = collision_distance(ray, game_object, agent_radius);

	if(distance <= 1.0f || distance == INFINITY)
	{
		return false;
	}

	return true;
}

// this function DOES NOT WORK. It's for octree intersections.
bool intersection_occurs(Ray ray, Octree otree, float agent_radius)
{
	glm::vec3 top_front_left = glm::vec3(otree.position.x - otree.half_dimension.x,
		otree.position.y + otree.half_dimension.y, 
		otree.position.z - otree.half_dimension.z);

	glm::vec3 top_back_left = glm::vec3(otree.position.x - otree.half_dimension.x,
		otree.position.y + otree.half_dimension.y, 
		otree.position.z + otree.half_dimension.z);

	glm::vec3 top_front_right = glm::vec3(otree.position.x + otree.half_dimension.x,
		otree.position.y + otree.half_dimension.y, 
		otree.position.z - otree.half_dimension.z);

	glm::vec3 top_back_right = glm::vec3(otree.position.x + otree.half_dimension.x,
		otree.position.y + otree.half_dimension.y,
		otree.position.z + otree.half_dimension.z);

	glm::vec3 bottom_front_left = glm::vec3(otree.position.x - otree.half_dimension.x,
		otree.position.y - otree.half_dimension.y,
		otree.position.z - otree.half_dimension.z);

	glm::vec3 bottom_back_left = glm::vec3(otree.position.x - otree.half_dimension.x,
		otree.position.y - otree.half_dimension.y,
		otree.position.z + otree.half_dimension.z);

	glm::vec3 bottom_front_right = glm::vec3(otree.position.x + otree.half_dimension.x,
		otree.position.y - otree.half_dimension.y,
		otree.position.z - otree.half_dimension.z);

	glm::vec3 bottom_back_right = glm::vec3(otree.position.x + otree.half_dimension.x,
		otree.position.y - otree.half_dimension.y,
		otree.position.z + otree.half_dimension.z);	


	// the following based on scratchapixel
	double tminx = (otree.position.x - otree.half_dimension.x - ray.position.x) / ray.direction.x;
	double tmaxx = (otree.position.x + otree.half_dimension.x - ray.position.x) / ray.direction.x;

	if(tminx > tmaxx)
	{
		std::swap(tminx, tmaxx);
	}

	float tminy = (otree.position.y - otree.half_dimension.y - ray.position.y) / ray.direction.y;
	float tmaxy = (otree.position.y + otree.half_dimension.y - ray.position.y) / ray.direction.y;
	
	if(tminy > tmaxy)
	{
		std::swap(tminy, tmaxy);
	}

	if(tminx > tmaxy || tminy > tmaxx)
	{
		return false;
	}

	if(tminy > tminx)
	{
		tminx = tminy;
	}

	if(tmaxy < tmaxx)
	{
		tmaxx = tmaxy;
	}

	float tminz = (otree.position.z - otree.half_dimension.z - ray.position.z) / ray.direction.z;
	float tmaxz = (otree.position.z + otree.half_dimension.z - ray.position.z) / ray.direction.z;

	if(tminz > tmaxz)
	{
		std::swap(tminz, tmaxz);
	}

	if(tminx > tmaxz || tminz > tmaxx)
	{
		return false;
	}

	return true;
}

#endif