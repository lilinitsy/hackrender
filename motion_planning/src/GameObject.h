#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <cstring>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "model.h"

class GameObject
{
	public:
		glm::vec3 position;
		float radius;
		bool can_move;

		GameObject()
		{
			position = glm::vec3(2.0f, 1.0f, 2.0f);
			can_move = false;
		}

		GameObject(glm::vec3 p, bool cm)
		{
			position = p;
			can_move = cm;
		}

		virtual void translate(glm::vec3 new_pos)
		{
			position = new_pos;
		}

		virtual void to_string()
		{
			printf("Position: (%f, %f, %f)\n", position.x, position.y, position.z);
		}
};














#endif
