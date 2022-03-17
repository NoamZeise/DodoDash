#ifndef MAP_OBJ_H
#define MAP_OBJ_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <timer.h>

#include "vulkan-render/render.h"
#include "animation.h"
#include "vulkan-render/texture_loader.h"
#include <glmhelper.h>
#include "gamehelper.h"

class MapObj
{
protected:
	glm::vec4 drawRect;
private:
	Animation animation;
	Frame currentFrame;
	bool active = true;
	glm::mat4 modelMat;
public:
	MapObj() {}
	MapObj(Animation animation)
	{
		this->animation = animation;
	}
	void Update(Timer &timer, glm::vec4 cameraRect)
	{
		active = gh::colliding(drawRect, cameraRect);
		if(active)
			currentFrame = animation.PingPong(timer);
	}
	void Draw(Render &render)
	{
		if(active)
			render.DrawQuad(currentFrame.tex, modelMat, glm::vec4(1.0f), currentFrame.textureOffset);
	}
	void setDrawRect(glm::vec4 drawRect, float scale)
	{
		this->drawRect = drawRect * scale;
		modelMat = glmhelper::calcMatFromRect(this->drawRect, 0.0f, 1.0f);
	}

	glm::vec4 getHitBox()
	{
		return drawRect;
	}
};


class MapGoal: public MapObj
{
public:
	MapGoal() : MapObj() {}
	MapGoal(Render &render)
		: MapObj(Animation(render.LoadTexture("textures/onMap/goal.png"), 100.0f, 200, false))
	{
		
	}
};

class Fruit: public MapObj
{
public:
	Fruit() : MapObj() {}
	Fruit(Render &render)
		: MapObj(Animation(render.LoadTexture("textures/onMap/fruit.png"), 100.0f, 100, false))
	{
		
	}
};

#endif