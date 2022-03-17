#ifndef BULLET_H
#define BULLET_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <timer.h>
#include <input.h>

#include "vulkan-render/render.h"
#include "animation.h"
#include "vulkan-render/texture_loader.h"
#include <glmhelper.h>
#include "gamehelper.h"


class Bullet
{
public:
	Bullet() {}
	Bullet(Render &render, float scale);

	void Update(Timer &timer, glm::vec4 cameraRect);
	void Draw(Render &render);

	void modBullet(glm::vec2 velocity, glm::vec2 position);
	glm::vec4 getHitBox() { return hitBox; }
private:
	struct BulletAnims
	{
		Animation shootLeft;
		Animation shootRight;
	};

	BulletAnims animations;
	Animation currentAnimation;
	Frame currentFrame;

	glm::mat4 modelMat;
	glm::vec4 drawRect;
	glm::vec4 hitBoxOffset;
	glm::vec2 velocity;
	glm::vec4 hitBox;

	float scale = 1.0f;
};


#endif