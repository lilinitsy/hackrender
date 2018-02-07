#ifndef BALL_BOUNCE_H
#define BALL_BOUNCE_H

#include "glad/glad.h"  //Include order can matter here
#ifdef __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
 #include <SDL_mixer.h>
#endif

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"

#include "Shader.h"
#include "model.h"
#include "utils.h"
#include "Ball.h"
#include "World.h"

Ball run_ball_sim(Ball ball, Shader shader, World world, float delta_time)
{
    
	
    if(ball.position.y <= 0.0f)
	{
		ball.acceleration.y = world.gravity.y;
		ball.velocity.y *= -0.95f;
		ball.velocity.x *= 0.45f;
		ball.velocity.z *= 0.45f;
	}

    ball.add_force(world.wind, delta_time);
    ball.update_velocity(delta_time);

	return ball;
}

void draw_ball(Ball ball, Shader shader)
{
    glm::mat4 model2 = setModel(ball.position, glm::vec3(0.005f, 0.005f, 0.005f), ball.rotation_radians, ball.rotation);
	shader.setMat4("model", model2);
	shader.setVec4("inColour", glm::vec4(0.2f, 0.3f, 0.8f, 0.8f));
    ball.draw(shader);
}


#endif
