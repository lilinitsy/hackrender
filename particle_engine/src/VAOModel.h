#ifndef VAOMODEL_H
#define VAOMODEL_H

#include <vector>

#include "glad/glad.h"  //Include order can matter here
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"

struct VAOModel
{
    std::vector<float> vertices;
    GLuint vao;
    GLuint vbo;

    void init()
    {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    void draw(Shader shader)
    {
        glBindVertexArray(vao);
        shader.setMat4("model", glm::mat4());
        shader.setVec4("inColour", glm::vec4(0.2f, 0.8f, 0.5f, 0.3f));

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
};




#endif