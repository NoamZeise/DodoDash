#include "bullet.h"

Bullet::Bullet(Render &render, float scale)
{
	animations.shootRight = Animation(render.LoadTexture("textures/enemy/bullet.png"), 100.0f, 162, true);
	animations.shootLeft = Animation(render.LoadTexture("textures/enemy/bullet.png"), 100.0f, 162, false);
	drawRect = glm::vec4(0, 0, 162, 38);
	drawRect *= scale;
	hitBoxOffset = glm::vec4(20, 10, 120, 20);
	hitBoxOffset *= scale;
}

void Bullet::Update(Timer &timer, glm::vec4 cameraRect)
{
	drawRect.x += timer.FrameElapsed() * velocity.x;
	drawRect.y += timer.FrameElapsed() * velocity.y;

	hitBox = drawRect + hitBoxOffset;
	hitBox.z = hitBoxOffset.z;
	hitBox.w = hitBoxOffset.w;

	currentFrame = currentAnimation.Play(timer);
	modelMat = glmhelper::calcMatFromRect(drawRect, 0.0f, 1.0f);

}

void Bullet::Draw(Render &render)
{
	render.DrawQuad(currentFrame.tex, modelMat, glm::vec4(1.0f), currentFrame.textureOffset);
}

void Bullet::modBullet(glm::vec2 velocity, glm::vec2 position)
{
	this->velocity = velocity;
	this->drawRect.x = position.x - drawRect.z/2;
	this->drawRect.y = position.y - drawRect.w/2;
	if(velocity.x > 0)
		currentAnimation = animations.shootRight;
	else
		currentAnimation = animations.shootLeft;
}