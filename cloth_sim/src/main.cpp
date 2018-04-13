#include "glad/glad.h"  //Include order can matter here
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

#include "Ball.h"
#include "Cloth.h"
#include "Mesh.h"
#include "Player.h"
#include "Shader.h"
#include "utils.h"

using namespace std;



Cloth set_default_cloth(int dimension);
Mesh set_default_ball();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window, float delta_time);


bool fmouse = 0;
bool pause = true;

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
// timing
float lastFrame = 0.0f;

Player *player = new Player(glm::vec3(-1.0f, 1.0f, -5.0f));


int main(int argc, char *argv[])
{
    int screen_width = 1280;
    int screen_height = 1080;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screen_height, screen_height, "cloth_simulation_pointmass_draw", NULL, NULL);
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

    Options options;
    Shader default_shader("shaders/vertexModel.glsl", "shaders/fragmentModel.glsl");
    default_shader.use();

    float planeVertices[] =
    {
        100.0f, -0.5f, 100.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 100.0f,    0.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,

        100.0f, -0.5f, 100.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,
        100.0f, -0.5f, 0.0f,    2.0f, 2.0f
    };

    Mesh *plane = new Mesh(planeVertices, 30, false, true);   
    plane->model = glm::mat4();
    plane->in_colour = glm::vec4(0.2f, 0.8f, 0.5f, 0.9f);

	SDL_Event windowEvent;
	bool quit = false;

    float current_time = SDL_GetPerformanceCounter();	// time between current frame and last frame
    float last_time = 0;

    Cloth cloth = set_default_cloth(30);

    Ball ball("bally", 10.0f);
    ball.mesh = set_default_ball();
    ball.mesh.model = glm::mat4();
    ball.radius = 0.375f;
    ball.origin = glm::vec3(0.0f, 0.0f, 0.0f);
    ball.mesh.in_colour = glm::vec4(0.8f, 0.3f, 0.3f, 0.8f);


    cloth.draw(default_shader);

	while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        current_time = SDL_GetTicks();
        float delta_time = 0.004f;// (float)(current_time - last_time) / 5000.0f;
        float dt = (float)(current_time - last_time) / 1000.0f;
        last_time = current_time;
        float move = delta_time * 50.0f;

        bool validEvent = false;
        // printf("fps: %f\n", 1.0f / dt);

        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        float cameraSpeed = 1.0f * delta_time;
        
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            player->camera_pos = glm::vec3(player->camera_pos.x + cameraSpeed * player->camera_front.x, 
                player->camera_pos.y + cameraSpeed * player->camera_front.y,
                player->camera_pos.z + cameraSpeed * player->camera_front.z);
        }

        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            player->camera_pos = glm::vec3(player->camera_pos.x - cameraSpeed * player->camera_front.x,
                player->camera_pos.y - cameraSpeed * player->camera_front.y, 
                player->camera_pos.z - cameraSpeed * player->camera_front.z);
        }

        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            player->camera_pos.y += cameraSpeed * 0.4f;
        }

        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            player->camera_pos.y -= cameraSpeed * 0.4f;
        }

        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            player->camera_pos -= glm::normalize(glm::cross(player->camera_front, player->camera_up)) * cameraSpeed;
        }

        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            player->camera_pos += glm::normalize(glm::cross(player->camera_front, player->camera_up)) * cameraSpeed;
        }


        if(glfwGetKey(window, GLFW_KEY_I) == GLFW_REPEAT)
        {
            for(int i = 0; i < cloth.width; i++)
            {
                cloth.point_mass[i][0]->position += 0.1f;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        {
            for(int i = 0; i < cloth.width; i++)
            {
                cloth.point_mass[i][0]->position -= 0.1f;
            }
        }
        
        if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            for(int i = 0; i < cloth.height; i++)
            {
                cloth.point_mass[0][i]->position += 0.1f;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        {
            for(int i = 0; i < cloth.height; i++)
            {
                cloth.point_mass[0][i]->position -= 0.1f;
            }
        }

        if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            pause = !pause;
        }



        glm::mat4 view = glm::lookAt(player->camera_pos, player->camera_pos + player->camera_front, player->camera_up);
        glm::mat4 proj = glm::perspective(3.14f/4, screen_width / (float) screen_height, 0.01f, 100.0f); //FOV, aspect, near, far
        default_shader.setMat4("projection", proj);
        default_shader.setMat4("view", view);
        default_shader.use();
    
        if(!pause)
        {
            ball.mesh.model = glm::mat4();
            cloth.update(delta_time, ball);
        }
        cloth.draw(default_shader);
        //ball.mesh.draw(default_shader);
        glfwSwapBuffers(window);
        glfwPollEvents();

	}

    glfwTerminate();

	return 0;
}

Cloth set_default_cloth(int dimension)
{
    Cloth cloth(dimension, dimension);

    for(int i = 0; i < dimension; i++)
    {
        for(int j = 0; j < dimension; j++)
        {
            float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 0.05f);
            PointMass *pm = new PointMass(glm::vec3(1.0f + (float) i / dimension, 1.0f - (float) j / dimension, 1.0f + z));
            cloth.add_point_mass(pm);
        }
    }

    cloth.init();
    cloth.mesh.model = glm::mat4();
    cloth.mesh.in_colour = glm::vec4(0.5f, 0.2f, 0.8f, 0.9f);

    return cloth;
}


Mesh set_default_ball()
{
    float mesh_verts[] = 
    {
        -0.25f,-0.25f,-0.25f, // triangle 1 : begin
        -0.25f,-0.25f, 0.25f,
        -0.25f, 0.25f, 0.25f, // triangle 1 : end
        0.25f, 0.25f,-0.25f, // triangle 2 : begin
        -0.25f,-0.25f,-0.25f,
        -0.25f, 0.25f,-0.25f, // triangle 2 : end
        0.25f,-0.25f, 0.25f,
        -0.25f,-0.25f,-0.25f,
        0.25f,-0.25f,-0.25f,
        0.25f, 0.25f,-0.25f,
        0.25f,-0.25f,-0.25f,
        -0.25f,-0.25f,-0.25f,
        -0.25f,-0.25f,-0.25f,
        -0.25f, 0.25f, 0.25f,
        -0.25f, 0.25f,-0.25f,
        0.25f,-0.25f, 0.25f,
        -0.25f,-0.25f, 0.25f,
        -0.25f,-0.25f,-0.25f,
        -0.25f, 0.25f, 0.25f,
        -0.25f,-0.25f, 0.25f,
        0.25f,-0.25f, 0.25f,
        0.25f, 0.25f, 0.25f,
        0.25f,-0.25f,-0.25f,
        0.25f, 0.25f,-0.25f,
        0.25f,-0.25f,-0.25f,
        0.25f, 0.25f, 0.25f,
        0.25f,-0.25f, 0.25f,
        0.25f, 0.25f, 0.25f,
        0.25f, 0.25f,-0.25f,
        -0.25f, 0.25f,-0.25f,
        0.25f, 0.25f, 0.25f,
        -0.25f, 0.25f,-0.25f,
        -0.25f, 0.25f, 0.25f,
        0.25f, 0.25f, 0.25f,
        -0.25f, 0.25f, 0.25f,
        0.25f,-0.25f, 0.25f
    };

    Mesh ball_mesh(mesh_verts, sizeof(mesh_verts) / sizeof(float), false, false);
    return ball_mesh;
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