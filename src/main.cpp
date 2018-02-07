#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
 #include <SDL_mixer.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <ctime>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Shader.h"
#include "model.h"
#include "Player.h"
#include "utils.h"
#include "Ball.h"
#include "World.h"
#include "ball_bounce.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "KillableParticleSystem.h"
#include "VAOModel.h"

using namespace std;

int screenWidth = 800;
int screenHeight = 600;
bool fmouse = 0; // no idea what this is for
float yaw = -90.0f;
float pitch = 0.0f;

GLuint loadTexture(const char *path);
glm::mat4 setModel(glm::vec3 translation, glm::vec3 scale, float rotationRadians, glm::vec3 rotate);
bool collision(GameObject game_object, float vertices[]);


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    SDL_Window* window = SDL_CreateWindow("CSCI 5611 Homework 1", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);

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

    // AUDIO
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;
    SDL_LoadWAV("../sounds/rickroll.wav", &wavSpec, &wavBuffer, &wavLength);
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    int soundSuccess = SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);

    World world;
    Option options;
    Player player(glm::vec3(5.0f, 0.0f, -2.0f));


    Shader modelShader("shaders/vertexModel.glsl", "shaders/fragmentModel.glsl");

    Ball ball = Ball("ball", 5.0f);
    ball.position = glm::vec3(4.4f, 2.0f, 2.9f);
    ball.rotation_radians = 0.0f;
    ball.model = Model("../models/sphere/sphere.obj");
    ball.acceleration = world.gravity;

    modelShader.use();

    VAOModel plane;

    float planeVertices[] =
    {
        100.0f, -0.5f, 100.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 100.0f,    0.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,

        100.0f, -0.5f, 100.0f,  2.0f, 0.0f,
        0.0f, -0.5f, 0.0f,      0.0f, 2.0f,
        100.0f, -0.5f, 0.0f, 2.0f, 2.0f
    };

    GLuint planeVAO;
    GLuint planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));



    SDL_Surface *surface = SDL_LoadBMP("../textures/stephen.bmp");
    if (surface==NULL)
    { //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }

    GLuint tex0;
    glGenTextures(1, &tex0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);

    //What to do outside 0-1 range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Load the texture into memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);


    SDL_FreeSurface(surface);


    
    ParticleSystem fireparticle_system = ParticleSystem(0.3f, glm::vec3(6.4f, 0.0f, 3.9f),
                                            1000.0f, 20.0f, 1.2f, 0.3f,
                                            glm::vec3(0.0f, .45f, 0.0f), glm::vec3(0.0f, 0.3f, 0.0f),
                                            glm::vec3(1.0f, 0.1f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), 1.2f,
                                            glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.002f, 0.002f, 0.002f), 0.00002f,
                                            1.0f, 0.5f, Model("../models/cube/cube.obj"), false, "fire",
                                            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true);
    world.particle_systems.push_back(fireparticle_system);

    ParticleSystem waterparticle_system = ParticleSystem(0.03f, glm::vec3(7.5f, 2.0f, 4.6f),
                                            100.0f, 20.0f, 12.2f, 0.1f,
                                            glm::vec3(0.4f, 3.0f, 0.4f), glm::vec3(0.2f, 0.1f, 0.2f),
                                            glm::vec3(0.1f, 0.1f, 1.0f), glm::vec3(0.1f, 0.1f, 0.1f), 0.05f,
                                            glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.002f, 0.002f, 0.002f), 0.00002f,
                                            6.0f, 0.5f, Model("../models/cube/cube.obj"), false, "water",
                                            glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), false);
    world.particle_systems.push_back(waterparticle_system);

    KillableParticleSystem fireworkparticle_system = KillableParticleSystem(0.5f, glm::vec3(2.0f, 0.0f, 1.0f),
                                            200.0f, 50.0f, 1.2f, 0.8f,
                                            glm::vec3(.1f, 0.05f, 0.1f), glm::vec3(0.0f, 0.05f, 0.0f),
                                            glm::vec3(0.2f, 0.8f, 0.8f), glm::vec3(0.1f, 0.01f, 0.1f), 1.2f,
                                            glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.002f, 0.002f, 0.002f), 0.00002f,
                                            3.0f, 0.5f, Model("../models/cube/cube.obj"), false, "fireworks",
                                            glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true, 2.0f);
    world.particle_systems.push_back(fireworkparticle_system);


	SDL_Event windowEvent;
	bool quit = false;

    float current_time = SDL_GetPerformanceCounter();	// time between current frame and last frame
    float last_time = 0;
    bool start_sim = false;

    bool fireball = true;

    bool pause = true;


	while (!quit)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     //   glEnable(GL_BLEND);
    //	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        options.particle_system_options = ParticleSystem_Option();

        current_time = SDL_GetTicks();
        float delta_time = (float)(current_time - last_time) / 5000.0f;
        // / 1000.0f is normal speed
        last_time = current_time;

        bool validEvent = false;
        float move = delta_time * 50.0f;

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

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT)
            {
                yaw -= 2.0f;
                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                player.camera_front = glm::normalize(front);
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT)
            {
                yaw += 2.0f;
                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                player.camera_front = glm::normalize(front);
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP)
            {
                pitch += 2.0f;
                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                player.camera_front = glm::normalize(front);
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN)
            {
                pitch -= 2.0f;
                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                player.camera_front = glm::normalize(front);
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_SPACE)
            {
                player.camera_pos = glm::vec3(player.camera_pos.x + player.camera_front.x * move, player.camera_pos.y + player.camera_up.y * move, player.camera_pos.z + player.camera_front.z * move);
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LSHIFT)
            {
                player.camera_pos = glm::vec3(player.camera_pos.x + player.camera_front.x * move, player.camera_pos.y - player.camera_up.y * move, player.camera_pos.z + player.camera_front.z * move);
            }

            else if(windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_g)
            {
                printf("Camera position (%f, %f, %f)\n", player.camera_pos.x, player.camera_pos.y, player.camera_pos.z);
                ball.to_string();
                start_sim = true;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_y)
            {
                waterparticle_system.spawn_position.y += 0.1f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_h)
            {
                waterparticle_system.spawn_position.y -= 0.1f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_u)
            {
                waterparticle_system.spawn_position.x += 0.1f;
            }

            else if(windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_j)
            {
                waterparticle_system.spawn_position.x -= 0.1f;
            }

        }

        modelShader.use();

        glm::mat4 view = glm::lookAt(player.camera_pos, player.camera_pos + player.camera_front, player.camera_up);
        glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 0.01f, 100.0f); //FOV, aspect, near, far
        modelShader.setMat4("projection", proj);
        modelShader.setMat4("view", view);

        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);

        glBindVertexArray(planeVAO);
        modelShader.setMat4("model", glm::mat4());
        modelShader.setVec4("inColour", glm::vec4(0.2f, 0.8f, 0.5f, 0.9f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        

        if(fireball)
        {
            for(int i = 0; i < world.particle_systems.size(); i++)
            {
                if(world.particle_systems[i].type == "fire")
                {
                    fireparticle_system.spawn_position = ball.position;
                }
            }
        }

        if(options.ball_options.ball_sim)
        {
            //modelShader.setInt("tex", 1);
            //modelShader.setBool("use_texture", true);

            if(ball.position.y > -0.25f)
            {
                ball = run_ball_sim(ball, modelShader, world, delta_time);
            }

            draw_ball(ball, modelShader);
            //modelShader.setBool("use_texture", false);

        }
        

        if(1.0f / (delta_time * 10.0f) >= 30.0f)
        {
            for(int i = 0; i < world.particle_systems.size(); i++)
            {
                if(world.particle_systems[i].type == "fire")
                {
                    fireparticle_system.spawn_particles(delta_time);
                }

                if(world.particle_systems[i].type == "water")
                {
                    waterparticle_system.spawn_particles(delta_time);
                }
            }

            printf("Number of particles: %zu\n", waterparticle_system.particles.size() + fireparticle_system.particles.size() + fireworkparticle_system.particles.size());
            printf("fps: %f\n", 1.0f / (delta_time * 5.0f));
        }

        for(int i = 0; i < world.particle_systems.size(); i++)
        {
    
            if(world.particle_systems[i].type == "fire")
            {
                if(fireparticle_system.particles.size() > 10)
                {
                    fireparticle_system.check_collisions(waterparticle_system);
                }
                fireparticle_system.draw_and_update_particles(modelShader, delta_time);
            }   

            if(world.particle_systems[i].type == "water")
            {
                waterparticle_system.draw_and_update_particles(modelShader, delta_time);
            }
    
            /*
            if(world.particle_systems[i].type == "fireworks")
            {
                fireworkparticle_system.update_system(modelShader, delta_time);
                fireworkparticle_system.draw_and_update_particles(modelShader, delta_time);
            }   */
        }

        SDL_GL_SwapWindow(window);
	}

    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
    SDL_GL_DeleteContext(context);
	SDL_Quit();

	return 0;
}

/*
bool collision(GameObject game_object, float vertices[])
{
    int vertices_size = sizeof(vertices) / sizeof(vertices[0]);

    for(int i = 0; i < vertices_size; i++)
    {
        if(game_object.position )
    }

    return true;
}
*/

// from learnopengl.com
GLuint loadTexture(char const * path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
