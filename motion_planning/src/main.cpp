#include "glad/glad.h"  
#include <GLFW/glfw3.h>

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window, float delta_time);


bool fmouse = 0; // no idea what this is for
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
// timing
float lastFrame = 0.0f;

Player *player = new Player(glm::vec3(20.0f, 1.0f, 3.0f));

/* TODO:
    Monday:
        D* Lite - get it working with the Boids. Try with a Boid of size 1, then n.
        Fix the octree. Probably will just take GameObject, Agent, Obstacle, Octree
            to a new folder and test / bugfix on that.
        ???
*/



int main(int argc, char *argv[])
{
    int screen_width = 1280;
    int screen_height = 1080;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screen_height, screen_height, "CSCI5611_FINAL", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    float yaw = -90.0f;
    float pitch = 0.0f;

    if(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
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

    Octree octree(glm::vec3(50.0f, 0.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f));
    std::vector<Obstacle*> obstacle_list;

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

	while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        float delta_time = 0.013f * 0.3f;
        processInput(window, delta_time);
    
        glm::mat4 view = glm::lookAt(player->camera_pos, player->camera_pos + player->camera_front, player->camera_up);
        glm::mat4 proj = glm::perspective(3.14f/4, screen_width / (float) screen_height, 0.01f, 100.0f); //FOV, aspect, near, far
        default_shader.setMat4("projection", proj);
        default_shader.setMat4("view", view);
        default_shader.use();

        boid.update(octree, agent_octree, delta_time);
        boid.draw(default_shader);

        plane->draw(default_shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
	}

    glfwTerminate();

	return 0;
}

// from learnopengl.com
void processInput(GLFWwindow *window, float delta_time)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 1.0f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        player->camera_pos = glm::vec3(player->camera_pos.x + cameraSpeed * player->camera_front.x, 
            player->camera_pos.y + cameraSpeed * player->camera_front.y,
            player->camera_pos.z + cameraSpeed * player->camera_front.z);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        player->camera_pos = glm::vec3(player->camera_pos.x - cameraSpeed * player->camera_front.x,
            player->camera_pos.y - cameraSpeed * player->camera_front.y, 
            player->camera_pos.z - cameraSpeed * player->camera_front.z);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        player->camera_pos.y += cameraSpeed * 0.4f;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        player->camera_pos.y -= cameraSpeed * 0.4f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        player->camera_pos -= glm::normalize(glm::cross(player->camera_front, player->camera_up)) * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        player->camera_pos += glm::normalize(glm::cross(player->camera_front, player->camera_up)) * cameraSpeed;
    }

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (fmouse)
    {
        lastX = xpos;
        lastY = ypos;
        fmouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    player->camera_front = glm::normalize(front);
}