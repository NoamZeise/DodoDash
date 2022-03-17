#ifndef ANIMATION_H
#define ANIMATION_H

#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <glmhelper.h>
#include "vulkan-render/texture_loader.h"
#include "vulkan-render/vkhelper.h"
#include "timer.h"

struct Frame
{
	Resource::Texture tex;
	glm::vec4 textureOffset;
	glm::vec2 size;
	float delay;
};

class Animation
{
public:
	Animation(){};
	Animation(std::vector<Resource::Texture> textures, float delay)
	{
		for(const auto &tex: textures)
		{
			frames.push_back(Frame());
			frames.back().tex = tex;
			frames.back().textureOffset = glm::vec4(0, 0, 1, 1);
			frames.back().delay = delay;
			totalDuration += delay;
			frames.back().size = tex.dim;
		}
	}

	Animation(Resource::Texture texture, float delay, float FrameWidth)
	{
		frames.resize(texture.dim.x / FrameWidth);
		
		for(unsigned int i = 0; i < frames.size(); i++)
		{
			frames[i].tex = texture;
			frames[i].textureOffset = glmhelper::calcTexOffset
				(texture.dim, glm::vec4(i * FrameWidth, 0, FrameWidth, texture.dim.y));
			frames[i].delay = delay;
			totalDuration += delay;
			frames[i].size = glm::vec2(FrameWidth, texture.dim.y);
		}
	}

	Animation(Resource::Texture texture, float delay, float FrameWidth, bool invertX)
	{
		frames.resize(texture.dim.x / FrameWidth);
		
		for(unsigned int i = 0; i < frames.size(); i++)
		{
			frames[i].tex = texture;
			if(invertX)
			{
				frames[i].textureOffset = glmhelper::calcTexOffset
				(texture.dim, glm::vec4((i+1) * FrameWidth, 0, -FrameWidth, texture.dim.y));
			}
			else
			{
				frames[i].textureOffset = glmhelper::calcTexOffset
					(texture.dim, glm::vec4(i * FrameWidth, 0, FrameWidth, texture.dim.y));
			}
			frames[i].delay = delay;
			totalDuration += delay;
			frames[i].size = glm::vec2(FrameWidth, texture.dim.y);
		}
	}

	Animation(Resource::Texture texture, float delay, float FrameWidth, bool invertX, bool reverse)
	{
		if(texture.dim.x > FrameWidth)
			throw std::runtime_error("Frame width larger than texture width!");
		frames.resize(texture.dim.x / FrameWidth);
		
		for(unsigned int i = 0; i < frames.size(); i++)
		{
			frames[i].tex = texture;
			if(invertX)
			{
				frames[i].textureOffset = glmhelper::calcTexOffset
				(texture.dim, glm::vec4((i+1) * FrameWidth, 0, -FrameWidth, texture.dim.y));
			}
			else
			{
				frames[i].textureOffset = glmhelper::calcTexOffset
					(texture.dim, glm::vec4(i * FrameWidth, 0, FrameWidth, texture.dim.y));
			}
			frames[i].delay = delay;
			totalDuration += delay;
			frames[i].size = glm::vec2(FrameWidth, texture.dim.y);
		}
		if(reverse)
			std::reverse(frames.begin(), frames.end());
	}
	
	Animation(Resource::Texture texture, float delay, float FrameWidth, float FrameHeight, float yOffset, int frameCount)
	{
		frames.resize(frameCount);
		
		for(unsigned int i = 0; i < frames.size(); i++)
		{
			frames[i].tex = texture;
			frames[i].textureOffset = glmhelper::calcTexOffset
				(texture.dim, glm::vec4(i * FrameWidth, yOffset, FrameWidth, FrameHeight));
			frames[i].delay = delay;
			totalDuration += delay;
			frames[i].size = glm::vec2(FrameWidth, FrameHeight);
		}
	}

	Animation(std::vector<Frame> frames)
	{
		this->frames = frames;
		for(const auto &f: frames)
			totalDuration += f.delay;
	}

	std::vector<Frame> getAllFrames()
	{
		return frames;
	}

	Frame getFrame(int index)
	{
		return frames[index];
	}
	
	int getIndex()
	{
		return current;
	}

	void setFrame(unsigned int index)
	{
		if(index >= frames.size())
			current = frames.size() - 1;
		else
			current = index;
	}

	Frame Play(Timer &timer)
	{
		frameTimer += timer.FrameElapsed();
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			current++;
			if(current >= frames.size())
				current = 0;
		}
		return frames[current];
	}

	Frame PlayOnce(Timer &timer)
	{
		if(!done)
		{
			frameTimer += timer.FrameElapsed();
			if(frameTimer > frames[current].delay)
			{
				frameTimer = 0;
				current++;
				if(current >= frames.size())
				{
					done = true;
					current--;
				}
			}
		}
		return frames[current];
	}

	Frame PingPong(Timer &timer)
	{
		if(frames.size() == 1)
			return frames[0];
		frameTimer += timer.FrameElapsed();
		if(frameTimer > frames[current].delay)
		{
			frameTimer = 0;
			if(pingPonged)
			{
				current--;
				if(current == 0)
				{
					pingPonged = false;
				}	
			}
			else
			{
				current++;
				if(current >= frames.size())
				{
					pingPonged = true;
					current--;
				}
			}
		}
		return frames[current];
	}

	Frame PingPongOnce(Timer &timer)
	{
		if(frames.size() == 1)
			return frames[0];
		if(!done)
		{
			frameTimer += timer.FrameElapsed();
			if(frameTimer > frames[current].delay)
			{
				frameTimer = 0;
				if(pingPonged)
				{
					current--;
					if(current == 0)
					{
						done = true;
					}	
				}
				else
				{
					current++;
					if(current >= frames.size())
					{
						pingPonged = true;
						current--;
					}
				}
			}
		}
		return frames[current];
	}


	void Reset()
	{
		done = false;
		pingPonged = false;
		current = 0;
	}

	void setFromOld(Animation* animation)
	{
		done = animation->done;
		pingPonged = animation->pingPonged;
		current = animation->current;
	}

	float getTotalDuration()
	{
		return totalDuration;
	}

	int maxIndex() { return frames.size() - 1; }


private:
	unsigned int current = 0;
	std::vector<Frame> frames;
	float frameTimer = 0;
	float totalDuration = 0;
	bool done = false;
	bool pingPonged = false;
};


#endif