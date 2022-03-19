#ifndef CRAB_H
#define CRAB_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <vector>

#include <timer.h>

#include "vulkan-render/render.h"
#include "animation.h"
#include "vulkan-render/texture_loader.h"
#include <glmhelper.h>
#include "gamehelper.h"
#include "bullet.h"


class Crab
{
public:
	Crab(){}
	Crab(Render &render, float scale, glm::vec4 track);

	void setCrab(glm::vec4 position);

	void Update(Timer &timer, glm::vec4 cameraRect, glm::vec2 playerPos);
	void Draw(Render &render);

	bool isAlive() { return alive; }
	void kill()
	{
		alive = false;
	}

	glm::vec4 getHitBox() { return hitBox; }

private:
	struct CrabAnimations
	{
		Animation walkRight;
		Animation walkLeft;
	};

	CrabAnimations animations;
	Animation currentAnimation;
	Frame currentFrame;

	glm::mat4 modelMat;
	glm::vec4 drawRect;
	glm::vec4 hitBox;
	glm::vec4 track;
	float position = 0.0f;
	float speed = 0.1f;

	float scale = 1.0f;

	bool active = false;
	bool shooting = false;
	bool alive = true;
	bool isLeft = true;
};

#endif