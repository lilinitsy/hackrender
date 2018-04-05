#ifndef CLOTH_H
#define CLOTH_H


#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Mesh.h"
#include "PointMass.h"
#include "Shader.h"
#include "Triangle.h"

struct Cloth
{
    public:
        PointMass ***point_mass;
        PointMass ***old_pms;
        std::vector<PointMass*> point_masses;
        std::vector<Triangle> triangles;
        Mesh mesh;

        glm::vec3 gravity = glm::vec3(0.0f, -0.3f, 0.0f);
        glm::vec3 velocity_air = glm::vec3(-0.2f, 0.0f, -0.2f);
        float air_density = 0.0001f;
        float drag_coeff = 0.0003f;

        float rest_length = 0.02f;
        float k = 400.0f;
        float dampening = 5.0f;
        float mass = 1.0f / 5.0f;

        int width;
        int height;


        Cloth(int w, int h) 
        {
            width = w;
            height = h;
            point_mass = new PointMass**[w];
            
            for(int i = 0; i < w; i++)
            {
                point_mass[i] = new PointMass*[h];
            }

            old_pms = new PointMass**[width];
        
            for(int i = 0; i < width; i++)
            {
                for(int j = 0; j < height; j++)
                {
                    old_pms = point_mass;
                }
            }
        }


        void add_point_mass(PointMass *point_mass)
        {
            point_masses.push_back(point_mass);
        }


        void init()
        {
            int ka = 0;
            for(int i = 0; i < width; i++)
            {
                for(int j = 0; j < height; j++)
                {
                    point_mass[i][j] = point_masses[ka];
                    ka++;
                }
            }

            float mesh_verts[point_masses.size() * 3];

            for(int i = 0; i < point_masses.size() * 3; i += 3)
            {
                mesh_verts[i] = point_masses[i / 3]->position.x;
                mesh_verts[i + 1] = point_masses[i / 3]->position.y;
                mesh_verts[i + 2] = point_masses[i / 3]->position.z;
            }

            for(int i = 0; i < width - 1; i++)
            {
                for(int j = 0; j < height - 1; j++)
                {
                    Triangle triangle = Triangle(point_mass[i][j], point_mass[i][j + 1], point_mass[i + 1][j]);
                    printf("i, j: %d %d\n", i, j);
                    printf("i + 1, j + 1: %d %d\n", i + 1, j + 1);
                    triangles.push_back(triangle);
                }
            }

            mesh = Mesh(mesh_verts, point_masses.size() * 3, false, false); 
        }

        void update(float delta_time, Ball ball)
        {
            
            #pragma omp parallel for
            for(int i = 0; i < width - 1; i++)
            {
                for(int j = 0; j < height; j++)
                {

                    glm::vec3 e = old_pms[i + 1][j]->position - old_pms[i][j]->position;
                    float l = glm::length(e);
                    e = glm::normalize(e);

                    bool collision = check_collision(point_mass[i][j], ball);
                    bool self_collision = check_self_collision(point_mass[i][j]);        


                    if(collision)
                    {
                        sphere_collision_interaction(ball, i, j, delta_time);
                    }   

                    else if(self_collision)
                    {
                        point_mass[i][j]->velocity -= 0.4f;
                        point_mass[i][j]->position += point_mass[i][j]->velocity * delta_time;
                    }

                    else
                    {
                        float v1 = glm::dot(e, old_pms[i][j]->velocity);
                        float v2 = glm::dot(e, old_pms[i + 1][j]->velocity);
                        float force = -1.0f * k * (rest_length - l) - dampening * (v1 - v2);

                        point_mass[i][j]->velocity += (force / mass) * e * delta_time;
                        point_mass[i + 1][j]->velocity -= (force / mass) * e * delta_time;
                    }

                    
                }
            }

            #pragma omp parallel for
            for(int i = 0; i < width; i++)
            {
                for(int j = 0; j < height - 1; j++)
                {
                    glm::vec3 e = old_pms[i][j + 1]->position - old_pms[i][j]->position;
                    float l = glm::length(e);
                    e = glm::normalize(e);

                    bool collision = check_collision(point_mass[i][j], ball);
                    bool self_collision = check_self_collision(point_mass[i][j]);        

                    if(collision)
                    {
                        sphere_collision_interaction(ball, i, j, delta_time);
                    }

                    else if(self_collision)
                    {
                        point_mass[i][j]->velocity -= 0.4f;
                        point_mass[i][j]->position += point_mass[i][j]->velocity * delta_time;
                    }

                    else
                    {
                        float v1 = glm::dot(e, old_pms[i][j]->velocity);
                        float v2 = glm::dot(e, old_pms[i][j + 1]->velocity);
                        float force = -1.0f * k * (rest_length - l) - dampening * (v1 - v2);

                        point_mass[i][j]->velocity += 0.5f * (force / mass) * e * delta_time;
                        point_mass[i][j + 1]->velocity -= 0.5f * (force / mass) * e * delta_time;
                    }
                }
            }

            #pragma omp parallel for
            for(int i = 0; i < width; i++)
            {
                for(int j = 1; j < height; j++)
                {
                    point_mass[i][j]->velocity += gravity * delta_time;
                    point_mass[i][j]->position += point_mass[i][j]->velocity * delta_time;
                }
            }

            // pin the top row
            for(int i = 0; i < width; i++)
            {
                point_mass[i][0]->velocity = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            update_mesh();
        }

        void draw(Shader shader)
        {
            mesh.draw_points(shader);
        }

        void to_string()
        {
            for(int i = 0; i < width; i++)
            {
                for(int j = 0; j < height; j++)
                {
                    printf("i j %d %d ", i, j);
                    point_mass[i][j]->to_string();
                }
            }
            
            printf("\n");
        }

    private:
        void update_mesh()
        {
            for(int i = 0; i < point_masses.size() * 3; i += 3)
            {
                mesh.vertices[i] = point_masses[i / 3]->position.x;
                mesh.vertices[i + 1] = point_masses[i / 3]->position.y;
                mesh.vertices[i + 2] = point_masses[i / 3]->position.z;
            }
        }

        
        bool check_collision(PointMass *pm, Ball ball)
        {
            if(pm->position.x >= ball.origin.x - ball.radius &&
                pm->position.x <= ball.origin.x + ball.radius &&
                pm->position.y >= ball.origin.y - ball.radius &&
                pm->position.y <= ball.origin.y + ball.radius &&
                pm->position.z >= ball.origin.z - ball.radius &&
                pm->position.z <= ball.origin.z + ball.radius)
            {
                printf("SPHERE COLLISION DETECTED\n");
                return true;
            }

            return false;
        }

        
        bool check_self_collision(PointMass *pm)
        {
            for(int i = 0; i < width; i++)
            {
                for(int j = 0; j < height; j++)
                {
                    if(pm->position.x <= point_mass[i][j]->position.x + 0.001f &&
                        pm->position.x >= point_mass[i][j]->position.x - 0.001f &&
                        pm->position.y <= point_mass[i][j]->position.y + 0.001f &&
                        pm->position.y >= point_mass[i][j]->position.y - 0.001f &&
                        pm->position.z <= point_mass[i][j]->position.z + 0.001f &&
                        pm->position.z >= point_mass[i][j]->position.z - 0.001f &&
                        pm != point_mass[i][j])
                    {
                        printf("self collision detected\n");
                        return true;
                    }
                }
            }

            return false;
        }

        void sphere_collision_interaction(Ball ball, int i, int j, float delta_time)
        {
            glm::vec3 normal = -1.0f * (ball.origin - old_pms[i][j]->position);
            normal = glm::normalize(normal);
            
            glm::vec3 bounce = normal * glm::dot(old_pms[i][j]->velocity, normal);
            point_mass[i][j]->velocity -= 1.0f * bounce;
            point_mass[i][j]->position += normal * (0.1f + (ball.radius - (ball.origin - old_pms[i][j]->position))) * delta_time;
        }
};










#endif