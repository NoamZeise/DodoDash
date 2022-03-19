#include "crab.h"

Crab::Crab(Render &render, float scale, glm::vec4 track)
{
	this->scale = scale;

	animations.walkRight = Animation(render.LoadTexture("textures/enemy/crabWalk.png"), 80.0f, 320, false);
	animations.walkLeft = Animation(render.LoadTexture("textures/enemy/crabWalk.png"), 80.0f, 320, true);

	currentAnimation = animations.walkRight;

	setCrab(track);
}

void Crab::setCrab(glm::vec4 track)
{
	this->track = track * scale;
	drawRect = glm::vec4(this->track.x, this->track.y, this->track.w, this->track.w);
	hitBox = drawRect;
	hitBox.x += 20;
	hitBox.y += 0;
	hitBox.z = this->track.w - 40;
	hitBox.w = this->track.w;
	hitBox *= scale;
}

void Crab::Update(Timer &timer, glm::vec4 cameraRect, glm::vec2 playerPos)
{
	if(gh::colliding(drawRect, cameraRect))
		active = true;
	else
		active = false;
	if(active)
	{
		drawRect.x += speed * timer.FrameElapsed();
		hitBox.x += speed * timer.FrameElapsed();
		if(!gh::colliding(hitBox,  track))
		{
			speed = -speed;
			drawRect.x += 2*speed * timer.FrameElapsed();
			hitBox.x += 2*speed * timer.FrameElapsed();

			if(isLeft)
			{
				isLeft = false;
				currentAnimation = animations.walkRight;
			}
			if(!isLeft)
			{
				isLeft = true;
				currentAnimation = animations.walkLeft;
			}
		}
		modelMat = glmhelper::calcMatFromRect(drawRect, 0.0f, 1.0f);
		currentFrame = currentAnimation.Play(timer);
	}
}


void Crab::Draw(Render &render)
{
	if(active)
		render.DrawQuad(currentFrame.tex, modelMat, glm::vec4(1.0f), currentFrame.textureOffset);
}