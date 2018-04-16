#ifndef PLAYER_H
#define PLAYER_H

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include <string>
#include <vector>

/*
	Please don't modify me! Only one of me should exist.
*/

struct Player
{
	glm::vec3 camera_pos;
	glm::vec3 camera_front;
	glm::vec3 camera_up;
	glm::vec3 camera_right; 
	
	Player()
	{
		camera_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		camera_front = glm::vec3(0.0f, 0.0f, 1.0f);
		camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
		camera_right = glm::cross(camera_front * -1.0f, camera_up);
	}

	Player(glm::vec3 p)
	{
		camera_pos = p;
		camera_front = glm::vec3(0.0f, 0.0f, 1.0f);
		camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
		camera_right = glm::cross(camera_front * -1.0f, camera_up);
	}
};

#endif
