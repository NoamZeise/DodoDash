#ifndef BACKGROUND_H
#define BACKGROUND_H

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

class Background
{
public:
	Background() {}
	Background(Resource::Texture bg, float height, float speed, float depth, glm::vec4 mapRect)
	{
		this->tex = bg;
		this->height = height;
		this->speed = speed;
		this->depth = depth;
		this->mapRect = mapRect;
	}

	void Update(Timer &timer, glm::vec4 camRect)
	{
		//currentMove += timer.FrameElapsed() * speed;
		//if(currentMove > tex.dim.x)
		//	currentMove = 0.0f;
		texOffset = glmhelper::calcTexOffset(tex.dim, glm::vec4(mapRect.x + currentMove, mapRect.y, mapRect.z, tex.dim.y));
		mat = glmhelper::calcMatFromRect(glm::vec4((int)camRect.x * speed, (int)camRect.y + height, mapRect.z, tex.dim.y), 0.0f, depth);
	}

	void Draw(Render &render)
	{
		render.DrawQuad(tex, mat, glm::vec4(1.0f), texOffset);
	}


private:
	Resource::Texture tex;
	float height;
	float speed;
	float depth;
	float currentMove = 0.0f;

	glm::vec4 mapRect;
	glm::mat4 mat;
	glm::vec4 texOffset;

};

#endif