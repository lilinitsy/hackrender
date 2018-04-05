#ifndef OCTREE_H
#define OCTREE_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "GameObject.h"

struct Octree
{
	Octree *children[8];
	std::vector<GameObject*> objects;
	glm::vec3 position;
	glm::vec3 half_dimension;
	bool is_leaf = true;

	Octree()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		half_dimension = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	Octree(glm::vec3 p, glm::vec3 hd)
	{
		position = p;
		half_dimension = hd;
	}

	void insert(GameObject *object)
	{
		if(is_leaf)
		{
			if(objects.size() <= 8)
			{
				objects.push_back(object);
				return;
			}

			else
			{
				std::vector<GameObject*> old_data = objects;
				objects.erase(objects.begin(), objects.end());

				for(int i = 0; i < 8; i++)
				{
					glm::vec3 new_position = position;
					new_position.x += half_dimension.x * (i & 4 ? -0.5f : 0.5f);
					new_position.y += half_dimension.y * (i & 2 ? -0.5f : 0.5f);
					new_position.z += half_dimension.z * (i & 1 ? -0.5f : 0.5f);
					children[i] = new Octree(new_position, half_dimension * 0.5f);
					is_leaf = false;
				}

				for(int i = 0; i < objects.size(); i++)
				{
					children[get_octant_containing_point(old_data[i]->position)]->insert(old_data[i]);
				}

				children[get_octant_containing_point(object->position)]->insert(object);
			}
		}

		else
		{
			children[get_octant_containing_point(object->position)]->insert(object);
		}
	}

	Octree octree_containing_point(glm::vec3 point)
	{
		if(is_leaf)
		{
			return *this;
		}

		Octree *ot = children[get_octant_containing_point(point)];
		return ot->octree_containing_point(point);
	}

	int get_octant_containing_point(glm::vec3 point)
	{
		int oct = 0; // if it returns -1, throw an error

		if(point.x >= position.x)
		{
			oct |= 4;
		}

		if(point.y >= position.y)
		{
			oct |= 2;
		}

		if(point.z >= position.z)
		{
			oct |= 1;
		}

		if(oct == 0)
		{
			printf("May be issue\n");
		}

		return oct;
	}

	void to_string()
	{
		printf("Position: (%f, %f, %f)\n", position.x, position.y, position.z);
		printf("Half dimension: (%f, %f, %f)\n", half_dimension.x, half_dimension.y, half_dimension.z);
		printf("Objects: \n");
		for(int i = 0; i < objects.size(); i++)
		{
			printf("\t");
			objects[i]->to_string();
		}
		
		if(!is_leaf)
		{
			for(int i = 0; i < 8; i++)
			{
				printf("\t");
				children[i]->to_string();
			}
		}
	}
};

bool operator==(Octree o1, Octree o2)
{
	if(o1.position == o2.position)
	{
		return true;
	}

	return false;
}

bool operator!=(Octree o1, Octree o2)
{
	if(o1.position == o2.position)
	{
		return false;
	}

	return true;
}

#endif