#include "glad/glad.h"  //Include order can matter here

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Agent.h"
#include "Boid.h"
#include "model.h"
#include "Obstacle.h"
#include "Octree.h"
#include "Player.h"
#include "Shader.h"
#include "utils.h"
#include "VAOMesh.h"

using namespace std;


int main(int argc, char *argv[])
{
    int screen_width = 1280;
    int screen_height = 1080;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_Window* window = SDL_CreateWindow("CSCI 5611 Homework 1", 100, 100, screen_width, screen_height, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    float yaw = -90.0f;
    float pitch = 0.0f;

    if (gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
		printf("\nOpenGL loaded\n");
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n\n", glGetString(GL_VERSION));
	}

	else
    {
		printf("ERROR: Failed to initialize OpenGL context.\n");
		return -1;
	}

    srand(time(NULL));

    Player player(glm::vec3(20.0f, 1.0f, 3.0f));
    Shader default_shader("shaders/vertexModel.glsl", "shaders/fragmentModel.glsl");
    default_shader.use();

    float planeVertices[] =
    {
        40.0f, -0.5f, 40.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 40.0f,    0.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,

        40.0f, -0.5f, 40.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,
        40.0f, -0.5f, 0.0f,    2.0f, 2.0f
    };

    VAOMesh *plane = new VAOMesh(planeVertices, 30, false, true);   
    plane->model = glm::mat4();
    plane->in_colour = glm::vec4(0.2f, 0.8f, 0.5f, 0.9f);

    Obstacle *test_sphere_obstacle = new Obstacle(glm::vec3(20.0f, 1.0f, 30.0f), 0.4f);
    test_sphere_obstacle->model = Model("../models/sphere/sphere.obj");
    

    Octree octree(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f));
    std::vector<Obstacle*> obstacle_list;
    obstacle_list.push_back(test_sphere_obstacle);

    Octree agent_octree(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f));
    std::vector<Agent*> agent_list;

    for(int i = 0; i < 200; i++)
    {
    	float x = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 30.0f));
        float y = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
        float z = 10.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 40.0f));

        Obstacle *obst = new Obstacle(glm::vec3(x, y, z), 0.4f);
        obst->model = Model("../models/sphere/sphere.obj");

        obstacle_list.push_back(obst);
        octree.insert(obst);
    }
    
    Obstacle *obst1 = new Obstacle(glm::vec3(20.0f, 1.0f, 38.0f), 0.4f);
    Obstacle *obst2 = new Obstacle(glm::vec3(19.0f, 1.0f, 38.0f), 0.4f);
    Obstacle *obst3 = new Obstacle(glm::vec3(21.0f, 1.0f, 38.0f), 0.4f);
    obst1->model = Model("../models/sphere/sphere.obj");
    obst2->model = Model("../models/sphere/sphere.obj");
    obst3->model = Model("../models/sphere/sphere.obj");

    obstacle_list.push_back(obst1);
    obstacle_list.push_back(obst2);
    obstacle_list.push_back(obst3);

    octree.insert(obst1);
    octree.insert(obst2);
    octree.insert(obst3);

    for(int i = 0; i < 20; i++)
    {
        float x = 19.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (21.0f - 19.0f)));
        float y = 0.5f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (1.5f - 0.5f)));
        float z = 9.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (11.0f - 9.0f)));

        Agent *test_agent = new Agent(glm::vec3(x, y, z), 0.1f);
        test_agent->model = Model("../models/sphere/sphere.obj");
        agent_octree.insert(test_agent);
        agent_list.push_back(test_agent);
    }

    Boid boid = Boid();
    boid.agents = agent_list;
    boid.obstacles = obstacle_list;
    boid.construct_graph(200, glm::vec3(21.0f, 1.0f, 39.0f), octree);
    
	SDL_Event windowEvent;
	bool quit = false;

    float current_time = SDL_GetPerformanceCounter();	// time between current frame and last frame
    float last_time = 0;

	while (!quit)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        current_time = SDL_GetTicks();
        float delta_time = 0.013f * 0.15f;
        float dt = (float)(current_time - last_time) / 1000.0f;
        last_time = current_time;
        float move = delta_time * 200.0f;

        bool validEvent = false;
        printf("fps: %f\n", 1.0f / dt);

        while(SDL_PollEvent(&windowEvent))
        {
            if(windowEvent.type == SDL_QUIT)
            {
                quit = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_w)
            {
                player.camera_pos = glm::vec3(player.camera_pos.x + player.camera_front.x * move, player.camera_pos.y + player.camera_front.y * move, player.camera_pos.z + player.camera_front.z * move);
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_s)
            {
                player.camera_pos = glm::vec3(player.camera_pos.x - player.camera_front.x * move, player.camera_pos.y - player.camera_front.y * move, player.camera_pos.z - player.camera_front.z * move);
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_a)
            {
                player.camera_pos -= glm::normalize(glm::cross(player.camera_front, player.camera_up)) * move;
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_d)
            {
                player.camera_pos += glm::normalize(glm::cross(player.camera_front, player.camera_up)) * move;
                validEvent = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_g)
            {
                printf("Player position: %f %f %f\n", player.camera_pos.x, player.camera_pos.y, player.camera_pos.z);
            }

        }

        glm::mat4 view = glm::lookAt(player.camera_pos, player.camera_pos + player.camera_front, player.camera_up);
        glm::mat4 proj = glm::perspective(3.14f/4, screen_width / (float) screen_height, 0.01f, 100.0f); //FOV, aspect, near, far
        default_shader.setMat4("projection", proj);
        default_shader.setMat4("view", view);
        default_shader.use();

        boid.update(octree, agent_octree, delta_time);
        boid.draw(default_shader);

        plane->draw(default_shader);
        SDL_GL_SwapWindow(window);
	}

    SDL_GL_DeleteContext(context); 
	SDL_Quit();

	return 0;
}