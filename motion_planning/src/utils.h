#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <tuple>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

/*
    utils is just assorted structs and functions. The only currently used one is setModel, which should not be modified.
    The rest can be destroyed or added to at will
*/

struct Ball_Bounce_Option
{
    float gravity = 9.8f;
    bool ball_sim = true;
};

struct ParticleSystem_Option
{
    vector<std::string> type;
    int num_particles_difference = 0;
    int x_velocity_local = 0;
    int x_translation_local = 0;
    int y_translation_local = 0;
    int z_translation_local = 0;

    ParticleSystem_Option()
    {
        
    }
};

struct Option
{
    Ball_Bounce_Option ball_options;
    ParticleSystem_Option particle_system_options;
};



glm::mat4 setModel(glm::vec3 translation, glm::vec3 scale, float rotationRadians, glm::vec3 rotate)
{
    glm::mat4 model;
    model = glm::translate(model, translation);
    model = glm::scale(model, scale);
    model = glm::rotate(model, rotationRadians, rotate);

    return model;
}


bool operator<(std::pair<float, float> key1, std::pair<float, float> key2)
{
	if(key1.first < key2.first ||
	  (key1.first == key2.first && key1.second < key2.second))
	{
		return true;
	}

	return false;
}

bool operator>(std::pair<float, float> key1, std::pair<float, float> key2)
{
	if(key1.first < key2.first ||
	  (key1.first == key2.first && key1.second < key2.second))
	{
		return false;
	}

	return true;
}

#endif
