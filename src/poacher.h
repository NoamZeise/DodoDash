#ifndef POACHER_H
#define POACHER_H

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

class Poacher
{
public:
	Poacher() {}
	Poacher(Render &render, float scale, glm::vec4 position);
	void setPoacher(glm::vec4 position);

	void Update(Timer &timer, glm::vec4 cameraRect, glm::vec2 playerPos, std::vector<Bullet> *bullets);
	void Draw(Render &render);

	bool isShooting() { return shooting; }
	bool isAlive() { return alive; }
	void kill();

	glm::vec4 getHitBox() { return hitBox; }
private:
	struct PoacherAnimations
	{
		Animation shootRight;
		Animation dieRight;
		Animation shootLeft;
		Animation dieLeft;
	};

	PoacherAnimations animations;
	Animation currentAnimation;
	Frame currentFrame;
	Bullet bullet;

	glm::mat4 modelMat;
	glm::vec4 drawRect;
	glm::vec4 hitBox;

	glm::vec4 hitboxOffset = glm::vec4(110, 120, 100, 110);

	float scale = 1.0f;

	float shootDelay = 2500.0f;
	float shootTimer = 0.0f;
	bool active = false;
	bool shooting = false;
	bool alive = true;
	bool isLeft = true;
	float shootSpeed = 0.8f;
	bool shot = false;
};







#endif