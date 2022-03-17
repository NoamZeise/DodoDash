#ifndef BUTTON_H
#define BUTTON_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <string>

#include "vulkan-render/render.h"
#include "vulkan-render/texture_loader.h"
#include "vulkan-render/texfont.h"
#include <glmhelper.h>
#include "gamehelper.h"

class Button
{
public:
	Button() {}
	Button(Resource::Texture buttonTex, Resource::Font* font, glm::vec4 rect, std::string text)
	{
		this->buttonTex = buttonTex;
		this->font = font;
		this->rect = rect;
		this->text = text;
		float width = Resource::Font::MeasureString(font, text, fontSize);
		textOffset.x = (buttonTex.dim.x - width) / 2;
		textOffset.y = (buttonTex.dim.y) / 1.5f;
	}
	void Update(glm::vec4 cameraRect, glm::vec2 mousePos)
	{
		camRect = glm::vec2(cameraRect.x, cameraRect.y);
		modelMat = glmhelper::calcMatFromRect(
				glm::vec4(
				rect.x + (int)cameraRect.x,
				rect.y + (int)cameraRect.y,
				rect.z, rect.w), 0.0f, 5.0f);
		
		if(gh::contains(mousePos, rect))
			colliding = true;
		else
			colliding = false;
	}
	void Draw(Render &render)
	{
		render.DrawString(font, text, glm::vec2(rect.x + (int)camRect.x + textOffset.x, rect.y + (int)camRect.y + textOffset.y), fontSize, 0, glm::vec4(1.0f), 6.0f);
		if(colliding)
			render.DrawQuad(buttonTex, modelMat, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
		else
			render.DrawQuad(buttonTex, modelMat);
	}

	bool active() { return colliding; }
private:
	Resource::Texture buttonTex;
	Resource::Font* font;

	glm::vec4 rect;
	glm::vec4 textOffset;
	std::string text;
	bool colliding = false;
	float fontSize = 60.0f;
	glm::vec2 camRect;
	glm::mat4 modelMat;
};

#endif