#ifndef UTILS_H
#define UTILS_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"


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


#endif
