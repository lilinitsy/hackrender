#ifndef WORLD_H
#define WORLD_H

#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "ParticleSystem.h"

struct World
{
	glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 wind = glm::vec3(5.0f, 0.0f, 3.0f);

	vector<ParticleSystem> particle_systems;
};

#endif
