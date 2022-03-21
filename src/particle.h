#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <timer.h>
#include <input.h>
#include <random.h>

#include <vector>
#include <iostream>

#include "vulkan-render/render.h"
#include "animation.h"
#include "vulkan-render/texture_loader.h"
#include <glmhelper.h>
#include "gamehelper.h"

class Particle
{
public:
	Particle() {}
	Particle(Resource::Texture texture, glm::vec4 rect, glm::vec2 velocity, float lifespan, glm::vec4 initialColour, glm::vec4 finalColour, float rotation, float slow, float dist);
	void Update(Timer &timer, glm::vec4 camRect, glm::vec4 deltaRect);
	void Draw(Render &render);
	bool isRemoved() { return lifetime > lifespan; }

private:
	Resource::Texture texture;
	glm::vec4 rect;
	glm::vec2 velocity;
	glm::vec2 currentVelocity;

	float rotation;
	float slow;
	float dist;
	glm::mat4 modelMat;

	float lifespan;
	float lifetime = 0.0f;

	glm::vec4 initialColour;
	glm::vec4 finalColour;
	glm::vec4 currentColour;

	bool active = false;
};

class ParticleManager
{
public:
	ParticleManager() {}
	ParticleManager(Render &render);
	void Update(Timer &timer, glm::vec4 camRect);
	void Draw(Render &render);
	void EmitFeather(glm::vec2 pos, glm::vec2 playerVelocity, float lifetime);
	void EmitRain(glm::vec2 pos);
private:
	std::vector<Particle> particles;
	Resource::Texture featherTex;
	Resource::Texture rainTex;
	GameRandom rand;

	float rainTimer = 0.0f;
	float rainDelay = 20.0f;

	glm::vec4 prevCam;
};


#endif