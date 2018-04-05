#ifndef UTILS_H
#define UTILS_H

#include <cstring>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

struct Shader_Options
{
    std::vector<std::string> names;
};

struct Options
{
    Shader_Options shader_options;
};


glm::mat4 set_model(glm::vec3 translation, glm::vec3 scale, float rotationRadians, glm::vec3 rotate)
{
    glm::mat4 model;
    model = glm::translate(model, translation);
    model = glm::scale(model, scale);
    model = glm::rotate(model, rotationRadians, rotate);

    return model;
}


glm::vec3 construct_linear_equation(glm::vec3 point_1, glm::vec3 point_2)
{
    
}


#endif
