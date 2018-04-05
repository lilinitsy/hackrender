#ifndef MESH_H
#define MESH_H

#include <SDL.h>
#include <SDL_opengl.h>

#include <vector>

#include "utils.h"
#include "Shader.h"

struct Mesh
{
    GLuint vao;
    GLuint vbo;
    std::vector<float> vertices;

    glm::mat4 model;
    glm::vec4 in_colour;

    int sqrt_vertices_size;

    Mesh()
    {
        
    }

    Mesh(float verts[], int size, bool has_normals, bool has_tex_coords)
    {
        for(int i = 0; i < size; i++)
        {
            vertices.push_back(verts[i]);
        }

        sqrt_vertices_size = sqrt(vertices.size());

        if(has_normals && has_tex_coords)
        {
            init_mesh_normals_texcoords(verts, size);
        }

        else if(has_normals && !has_tex_coords)
        {
            init_mesh_normals(verts, size);
        }

        else if(!has_normals && has_tex_coords)
        {
            init_mesh_texcoords(verts, size);
        }

        else
        {
            init_mesh(verts, size);
        }
        
        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    Mesh(float verts[], int width, int height)
    {
        init_mesh_cloth_preset(verts, width, height);
        glEnable(GL_PROGRAM_POINT_SIZE);

    }

    void init_mesh(float verts[], int size)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    void init_mesh_cloth_preset(float verts[], int width, int height)
    {
        // http://www.learnopengles.com/tag/triangle-strips/
        std::vector<float> tmp_verts = vertices;
        

    }

    void init_mesh_normals_texcoords(float verts[], int size)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }

    void init_mesh_normals(float verts[], int size)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    void init_mesh_texcoords(float verts[], int size)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);

    }

    void draw_points(Shader shader)
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);

        shader.setMat4("model", model);
        shader.setVec4("inColour", in_colour);
        //glDrawArrays(GL_POINTS, 0, vertices.size() / 3);
        glm::vec4 old_incolour = in_colour;

        for(int i = 0; i < vertices.size() / 3; i++)
        {   
            old_incolour.r -= 0.001f;
            old_incolour.g += 0.001f;
            old_incolour.b += 0.001f;     
            shader.setVec4("inColour", old_incolour);

            glDrawArrays(GL_POINTS, 0, i);
        }

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }
    
    void draw(Shader shader)
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);

        shader.setMat4("model", model);
        shader.setVec4("inColour", in_colour);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);

    }

    void draw_lines(Shader shader, int width, int height)
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);

        shader.setMat4("model", model);
        shader.setVec4("inColour", in_colour);
        
        for(int i = 0; i < vertices.size(); i += height)
        {
            glDrawArrays(GL_LINE_STRIP_ADJACENCY, i, i + 1);
        }

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);

    }
/*
    // have to deform the mesh
    void update_mesh_vertex(glm::vec3 new_position, int vertex_index)
    {
        for(int i = 0; i < sizeof(verts) / sizeof(float); i++)
        {
            vertices[i] = verts[i];
        }
    }
*/
    void to_string()
    {
        for(int i = 0; i < vertices.size(); i++)
        {
            printf("i %d\t%f\n", i, vertices[i]);

            if(i % 3 == 2)
            {
                printf("\n");
            }
        }

        printf("vao: %d\n", vao);
        printf("vbo: %d\n", vbo);

        printf("sizeof vertices: %zu\n", sizeof(vertices));
    }
};

#endif