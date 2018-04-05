#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <cstring>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"


class GameObject
{
	public:
		glm::vec3 position;
		glm::vec3 rotation;
		float rotation_radians;
		std::string name;

		GameObject()
		{
			name = "noname";
			position = glm::vec3(2.0f, 1.0f, 2.0f);
			rotation = glm::vec3(1.0f, 1.0f, 1.0f);
			rotation_radians = 3.14f;
		}

		GameObject(std::string n)
		{
			name = n;
			position = glm::vec3(0.0f, 1.0f, 0.0f);
			rotation = glm::vec3(1.0f, 1.0f, 1.0f);
			rotation_radians = 3.14f;
		}

		virtual void translate(glm::vec3 new_pos)
		{
			position = new_pos;
		}

		virtual void rotate(glm::vec3 new_rotation, float rr)
		{
			rotation = new_rotation;
			rotation_radians = rr;
		}
};














#endif
