#ifndef FREE_CAMERA_H
#define FREE_CAMERA_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>
#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

#include <input.h>
#include <timer.h>

#include "vulkan-render/config.h"
#include <glmhelper.h>
#include "gamehelper.h"

namespace camera
{
	static float getOffset(float target, float focus, float roomPos, float roomSize)
	{
		if(target > roomSize)
			return -roomSize/2 -roomPos;
		float min = roomPos + target/2;
		float max = roomPos + roomSize - target/2;
		if(focus > min && focus < max)
			return -focus;
		if(focus < min)
			return -min;
		if(focus > max)
			return -max;
		return -focus;
	}

	const unsigned int CAM2D_FLOAT = 250;
	class camera2D
	{
	public:
		void SetCameraOffset(glm::vec2 pos)
		{
			cameraArea = glm::vec4(pos.x - settings::TARGET_WIDTH/2, pos.y - settings::TARGET_HEIGHT/2,
			 settings::TARGET_WIDTH, settings::TARGET_HEIGHT);
			offset = glm::translate(glm::mat4(1.0f),
				 glm::vec3(-pos.x + settings::TARGET_WIDTH/2, -pos.y + settings::TARGET_HEIGHT/2, 0));
			previousOff.x = getOffset(settings::TARGET_WIDTH, pos.x, currentRect.x, currentRect.z);
			previousOff.y = getOffset(settings::TARGET_HEIGHT, pos.y, currentRect.y, currentRect.w);
		}

		void Target(glm::vec2 focus, Timer &timer);

		void setZoom(float val) { zoom = val; }


		glm::mat4 getViewMat()
		{
			return offset;
		}
		void setCameraRects(std::vector<glm::vec4> cameraRects)
		{
			this->cameraRects = cameraRects;
		}
		void setCameraMapRect(glm::vec4 mapRect)
		{
			this->mapRect = mapRect;
		}
		void clearCameraRects()
		{
			cameraRects.clear();
			mapRect = glm::vec4(0);
		}

		glm::vec4 getCameraArea()
		{
			return cameraArea;
		}

		glm::vec2 getCameraOffset()
		{
			return glm::vec2(cameraArea.x, cameraArea.y);
		}

		glm::vec4 currentRoom = glm::vec4(0);
	private:
		glm::mat4 offset = glm::mat4(1.0f);
		std::vector<glm::vec4> cameraRects;
		glm::vec4 mapRect =  glm::vec4(0);

		glm::vec4 currentRect = glm::vec4(0, 0, 0, 0);
		glm::vec2 previousOff = glm::vec2(0);

		glm::vec4 cameraArea = glm::vec4(0);
		float zoom = 1.0f;
	};


}	//namesapce end



#endif
