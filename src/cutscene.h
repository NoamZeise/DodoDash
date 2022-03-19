#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include <timer.h>
#include <input.h>

#include "vulkan-render/render.h"
#include "animation.h"
#include "vulkan-render/texture_loader.h"
#include <glmhelper.h>
#include "gamehelper.h"

#include <iostream>

class Cutscene
{
public:
	Cutscene() {}
	Cutscene( Resource::Font* font) 
	{ 
		this->font = font;
	}
	void Update(Timer &timer, Input &input)
	{
		if(currentScene.none)
		{
			if(index < scenes.size())
				currentScene = scenes[index];
			else
				done = true;
		}
		currentSceneTimer += timer.FrameElapsed();
		if(currentSceneTimer > currentScene.duration)
		{
			index++;
			currentScene = Scene();
		}
		for(auto& elem: currentScene.elems)
		{
			elem.Update(currentSceneTimer, currentScene.duration);
		}

		if(input.Keys[GLFW_KEY_Z])
		{
			pressingZ += timer.FrameElapsed();
			if(pressingZ > 500.0f)
				done = true;
		}
		else
			pressingZ = 0.0f;
	}
	void Draw(Render &render)
	{
		for(auto& elem: currentScene.elems)
		{
			elem.Draw(render, font);
		}
	}
	bool isOver()
	{
		return done;
	}
	void Reset()
	{
		currentSceneTimer = 0.0f;
		index = 0;
		done = false;
		pressingZ = 0.0f;
	}
protected:
    struct SceneElem
	{
		SceneElem() {}
		SceneElem(Resource::Texture tex, glm::vec4 initalPos, glm::vec4 finalPos, float depth, glm::vec4 colour)
		{
			this->colour = colour;
			this->texture = tex;
			this->initialPos = initalPos;
			this->finalPos = finalPos;
			this->isText = false;
			this->depth = depth;
		}
		SceneElem(std::vector<std::string> texts, glm::vec4 colour ,glm::vec4 initalPos, glm::vec4 finalPos, float depth)
		{
			this->colour = colour;
			this->texts = texts;
			this->initialPos = initalPos;
			this->finalPos = finalPos;
			this->isText = true;
			this->depth = depth;
		}
		void Update(float current, float duration)
		{
			float ratio = current / duration;
			currentRect = initialPos;
			currentRect.x -= ((initialPos.x - finalPos.x) * ratio);
			currentRect.y -= ((initialPos.y - finalPos.y) * ratio);
			currentRect.x = (int)currentRect.x;
			currentRect.y = (int)currentRect.y;
			modelMat = glmhelper::calcMatFromRect(currentRect, 0.0, depth);
		}
		void Draw(Render &render, Resource::Font *font)
		{
			if(isText)
			{	
				int index = 0;
				for(auto &txt: texts)
				{
					render.DrawString(font, txt, glm::vec2(currentRect.x, currentRect.y + (index * 50)), 50.0f, 0.0f, colour, depth);
					index++;
				}
			}
			else
			{
				render.DrawQuad(texture, modelMat, colour);
			}
		}
		Resource::Texture texture;
		std::vector<std::string> texts;
		glm::vec4 initialPos;
		glm::mat4 modelMat;
		glm::vec4 currentRect;
		glm::vec4 finalPos;
		glm::vec4 colour;
		bool isText = false;
		float depth = 0.0f;
	};
	struct Scene
	{
		Scene() {}
		Scene(std::vector<SceneElem> elems, float duration) { this->elems = elems; this->duration = duration; none = false;}
		float duration;
		std::vector<SceneElem> elems;
		bool none = true;
	};
	int index = 0;
	Scene currentScene;
	std::vector<Scene> scenes;
	float currentSceneTimer = 0.0f;
	Resource::Font *font;
	bool done = false;
	float pressingZ = 0.0f;
};

class Opening: public Cutscene
{
public:
	Opening() : Cutscene() {}
	Opening(Render &render, Resource::Font* font) : Cutscene(font)
	{
		scenes = {
				Scene(
						{
						SceneElem(
							{"The Last Of The Dodos", "Testing the cutscene", "last hope of the dodos", "chosen one", "We believe in you"}, 
							glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
							glm::vec4(10, 1000, 100, 100), 
							glm::vec4(10, 0, 100, 100),
							1.0f
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(0, 0, 1920, 1080), 
							glm::vec4(-1920, 0, 1920, 1080),
							0.0f,
							glm::vec4(1.0f)
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(1920, 0, 1920, 1080), 
							glm::vec4(0, 0, 1920, 1080),
							0.0f,
							glm::vec4(1.0f)
							),
							
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(0, 500, 1920, 1080), 
							glm::vec4(-960, 100, 1920, 1080),
							0.5f,
							glm::vec4(1.0f)
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(1920, 500, 1920, 1080), 
							glm::vec4(960, 100, 1920, 1080),
							0.5f,
							glm::vec4(1.0f)
							),

						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(-960, 800, 1920, 1080), 
							glm::vec4(0, 1000, 1920, 1080),
							1.5f,
							glm::vec4(1.0f)
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(960, 800, 1920, 1080), 
							glm::vec4(1920, 1000, 1920, 1080),
							1.5f,
							glm::vec4(1.0f)
							),

						SceneElem(
							render.LoadTexture("textures/ui/pixel.png"), 
							glm::vec4(0, 0, 1920, 1080), 
							glm::vec4(0, 0, 1920, 1080),
							-0.5f,
							glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
							),
						}, 
					20000.0f)};
	}
};



class Extinct: public Cutscene
{
public:
	Extinct() : Cutscene() {}
	Extinct(Render &render, Resource::Font* font) : Cutscene(font)
	{
		scenes = {
				Scene(
						{
						SceneElem(
							{"We are now all dead", "This is very sad", "we are extinct now", "I cry"}, 
							glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
							glm::vec4(10, 1000, 100, 100), 
							glm::vec4(10, 0, 100, 100),
							1.0f
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(0, 0, 1920, 1080), 
							glm::vec4(-1920, 0, 1920, 1080),
							0.0f,
							glm::vec4(1.0f)
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(1920, 0, 1920, 1080), 
							glm::vec4(0, 0, 1920, 1080),
							0.0f,
							glm::vec4(1.0f)
							),
						
						SceneElem(
							render.LoadTexture("textures/ui/pixel.png"), 
							glm::vec4(0, 0, 1920, 1080), 
							glm::vec4(0, 0, 1920, 1080),
							-0.5f,
							glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
							),
						}, 
					20000.0f)};
	}
};


class Victory: public Cutscene
{
public:
	Victory() : Cutscene() {}
	Victory(Render &render, Resource::Font* font) : Cutscene(font)
	{
		scenes = {
				Scene(
						{
						SceneElem(
							{"You saved the dodo!", "This is very good", "we are saved now", "I cry with joy"}, 
							glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
							glm::vec4(10, 1000, 100, 100), 
							glm::vec4(10, 0, 100, 100),
							1.0f
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(0, 0, 1920, 1080), 
							glm::vec4(-1920, 0, 1920, 1080),
							0.0f,
							glm::vec4(1.0f)
							),
						SceneElem(
							render.LoadTexture("textures/water.png"), 
							glm::vec4(1920, 0, 1920, 1080), 
							glm::vec4(0, 0, 1920, 1080),
							0.0f,
							glm::vec4(1.0f)
							),
						
						SceneElem(
							render.LoadTexture("textures/ui/pixel.png"), 
							glm::vec4(0, 0, 1920, 1080), 
							glm::vec4(0, 0, 1920, 1080),
							-0.5f,
							glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
							),
						}, 
					20000.0f)};
	}
};

#endif