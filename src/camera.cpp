#include "camera.h"

namespace camera
{

	void camera2D::Target(glm::vec2 focus, Timer &timer)
		{
				
			glm::vec2 transform = glm::vec2(0);

			if(mapRect.z == 0 && cameraRects.size() == 0)
			{
				transform.x = -focus.x;
				transform.y = -focus.y;
			}
			else if(cameraRects.size() == 0)
			{
				transform.x = getOffset(settings::TARGET_WIDTH, focus.x, mapRect.x, mapRect.z);
				transform.y = getOffset(settings::TARGET_HEIGHT, focus.y, mapRect.y, mapRect.w);
			}
			else
			{
				glm::vec4 newRect = glm::vec4(0);
				currentRoom = glm::vec4(0);
				for(const auto& rect: cameraRects)
				{
					if(gh::contains(focus, rect))
					{ 
						newRect = rect;
						currentRoom = rect;
						break;
					}
				}
				if(newRect == glm::vec4(0))
				{
					transform.x = -focus.x;
					transform.y = -focus.y;
				}
				else
				{
					if (newRect != currentRect)
						currentRect = newRect;
					transform.x = getOffset(settings::TARGET_WIDTH, focus.x, currentRect.x, currentRect.z);
					transform.y = getOffset(settings::TARGET_HEIGHT, focus.y, currentRect.y, currentRect.w);
				}

				if(mapRect != glm::vec4(0))
				{
					transform.x = getOffset(settings::TARGET_WIDTH, -transform.x, mapRect.x, mapRect.z);
					transform.y = getOffset(settings::TARGET_HEIGHT, -transform.y, mapRect.y, mapRect.w);
				}
			}

			if(previousOff.x != 0 || previousOff.y != 0)
			{
			previousOff += glm::vec2(((transform.x - previousOff.x)/CAM2D_FLOAT) * timer.FrameElapsed(),
				((transform.y - previousOff.y)/CAM2D_FLOAT)* timer.FrameElapsed());
			transform = previousOff;
			}

			cameraArea = glm::vec4(-transform.x - (settings::TARGET_WIDTH/zoom)/2, -transform.y - (settings::TARGET_HEIGHT/zoom)/2,
									(settings::TARGET_WIDTH/zoom), (settings::TARGET_HEIGHT/zoom));
			offset = glm::translate(glm::mat4(1.0f), glm::vec3((int)(transform.x + (settings::TARGET_WIDTH/zoom)/2),
				 					(int)(transform.y + (settings::TARGET_HEIGHT/zoom)/2), 0));
			offset = glm::scale(offset, glm::vec3(zoom, zoom, 1));
			previousOff = transform;
		}

} //namespace end