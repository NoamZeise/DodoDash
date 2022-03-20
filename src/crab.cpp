#include "crab.h"

Crab::Crab(Render &render, float scale, glm::vec4 track)
{
	this->scale = scale;

	animations.walkRight = Animation(render.LoadTexture("textures/enemy/monkeyWalk.png"), 150.0f, 320, false);
	animations.walkLeft = Animation(render.LoadTexture("textures/enemy/monkeyWalk.png"), 150.0f, 320, true);

	currentAnimation = animations.walkRight;

	setCrab(track);
}

void Crab::setCrab(glm::vec4 track)
{
	this->track = track * scale;
	drawRect = glm::vec4(this->track.x, this->track.y, this->track.w, this->track.w);
	hitBox = drawRect;
	hitBox.x += 135;
	hitBox.y += 200;
	hitBox.z = this->track.w - 270;
	hitBox.w = this->track.w - 200;
	hitBox *= scale;
}

void Crab::Update(Timer &timer, glm::vec4 cameraRect, glm::vec2 playerPos)
{
	if(gh::colliding(drawRect, cameraRect))
		active = true;
	else
		active = false;

	drawRect.x += speed * timer.FrameElapsed();
	hitBox.x += speed * timer.FrameElapsed();
	if(!gh::contains(glm::vec2(hitBox.x, hitBox.y),track))
	{
		speed = -speed;
		drawRect.x += 2*speed * timer.FrameElapsed();
		hitBox.x += 2*speed * timer.FrameElapsed();

		if(speed > 0)
		{
			isLeft = false;
			currentAnimation = animations.walkRight;
		}
		if(speed < 0)
		{
			isLeft = true;
			currentAnimation = animations.walkLeft;
		}
	}
	if(active)
	{
		modelMat = glmhelper::calcMatFromRect(drawRect, 0.0f, 1.0f);
		currentFrame = currentAnimation.Play(timer);
	}
}


void Crab::Draw(Render &render)
{
	//render.DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(hitBox, 0.0, 10.0f), glm::vec4(1.0f), currentFrame.textureOffset);
	if(active)
		render.DrawQuad(currentFrame.tex, modelMat, glm::vec4(1.0f), currentFrame.textureOffset);
}