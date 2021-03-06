#ifndef MENU_H
#define MENU_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <stdexcept>

#include <input.h>

#include "vulkan-render/render.h"
#include "vulkan-render/texture_loader.h"
#include "vulkan-render/texfont.h"
#include <glmhelper.h>

#include "button.h"

class Menu
{
public:
	Menu() {}
	Menu(Resource::Texture buttonTex, Resource::Font* font, std::vector<std::string> texts, glm::vec2 topLeft, glm::vec2 buttonSize, float seperation)
	{
		max = texts.size() - 1;
		select = buttonTex;
		position = topLeft;
		position.x -= buttonTex.dim.x/4;
		position.y += 25;
		this->seperation = seperation;
		buttons.resize(texts.size());
		for(int i = 0; i < buttons.size(); i++)
		{
			buttons[i] = Button(buttonTex, font, glm::vec4(topLeft.x, topLeft.y + (seperation * i), buttonSize.x, buttonSize.y), texts[i]);
		}
	}

	virtual void Update(glm::vec4 cameraRect, glm::vec2 mousePos, Input &input, Input &prevInput)
	{
		if(input.Keys[GLFW_KEY_UP] && !prevInput.Keys[GLFW_KEY_UP])
			selected++;
		
		if(input.Keys[GLFW_KEY_DOWN] && !prevInput.Keys[GLFW_KEY_DOWN])
			selected--;

		selected = selected < 0 ? max : selected > max ? 0 : selected;

		modelMat = glmhelper::calcMatFromRect(glm::vec4(position.x + (int)cameraRect.x, (position.y + seperation * selected) + (int)cameraRect.y, 50, 50), 0.0f, 9.0f);

		for(auto& btn: buttons)
			btn.Update(cameraRect, mousePos);
	}
	virtual void Draw(Render &render)
	{
		render.DrawQuad(select, modelMat);
		for(int i = 0; i < buttons.size(); i++)
		{
			buttons[i].Draw(render);
		}
	}

	bool activeSelected(unsigned int index)
	{
		if(index >= buttons.size())
			throw std::runtime_error("tried to reference menu button outside of range");
		return selected == index;
	}
	bool activeMouse(unsigned int index)
	{
		if(index >= buttons.size())
			throw std::runtime_error("tried to reference menu button outside of range");
		return buttons[index].active();
	}
	void zeroSelected() { selected = 0; }
private:
	std::vector<Button> buttons;
	glm::vec2 position;
	Resource::Texture select;
	glm::mat4 modelMat;
	float seperation ;
	int selected = 0;
	int max = 0;
};

class PauseMenu: public Menu
{
public:
	PauseMenu() : Menu() {}
	PauseMenu(Resource::Texture colourPixel, Resource::Texture buttonTex, Resource::Font* font):
		Menu(buttonTex, font, {"Resume", "Exit"}, glm::vec2(settings::TARGET_WIDTH/2 - 150, 300), glm::vec2(300, 100), 150) 
		{
			this->colourPixel = colourPixel;
		}

	void Update(glm::vec4 cameraRect, glm::vec2 mousePos, Input &input, Input &prevInput, Timer &timer)
	{
		fadeTimer += timer.FrameElapsed();
		fadeRatio = (fadeTimer < fadeDelay ? fadeTimer/fadeDelay : 1.0f);
		bgMat = glmhelper::calcMatFromRect(glm::vec4((int)cameraRect.x, (int)cameraRect.y, settings::TARGET_WIDTH, settings::TARGET_HEIGHT), 0.0f, 9.0f);
		Menu::Update(cameraRect, mousePos, input, prevInput);
		if(input.Keys[GLFW_KEY_Z] && !prevInput.Keys[GLFW_KEY_Z])
		{
			Reset();
			if(activeSelected(0))
				resumed = true;
			if(activeSelected(1))
				exit = true;
			zeroSelected();
		}
		else if (input.Buttons[GLFW_MOUSE_BUTTON_LEFT] && !prevInput.Buttons[GLFW_MOUSE_BUTTON_LEFT])
		{
			Reset();
			if(activeMouse(0))
				resumed = true;
			if(activeMouse(1))
				exit = true;
			zeroSelected();
		}
		else
		{
			resumed = false;
			exit = false;
		}
	}

	void Draw(Render &render) override
	{
		render.DrawQuad(colourPixel, bgMat, glm::vec4(0.0f, 0.0f, 0.0f, 0.8f * fadeRatio));
		Menu::Draw(render);
	}

	bool isResumed() 
	{
		return resumed; 
	}
	bool isExit() 
	{ 
		return exit;
	}

	void Reset()
	{
		fadeTimer = 0.0f;
	}

private:
	bool resumed = false;
	bool exit = false;
	Resource::Texture colourPixel;
	glm::mat4 bgMat;

	float fadeDelay = 150.0f;
	float fadeTimer = 0.0f;
	float fadeRatio;

};


#endif