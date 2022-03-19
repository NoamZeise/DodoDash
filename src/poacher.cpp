#include "poacher.h"

Poacher::Poacher(Render &render, float scale, glm::vec4 position)
{
	this->scale = scale;
	bullet = Bullet(render, 0.25f);

	animations.shootRight = Animation(render.LoadTexture("textures/enemy/poacherShoot.png"), 80.0f, 320, true);
	animations.dieRight = Animation(render.LoadTexture("textures/enemy/poacherDie.png"), 80.0f, 200, true);
	animations.shootLeft = Animation(render.LoadTexture("textures/enemy/poacherShoot.png"), 80.0f, 320, false);
	animations.dieLeft = Animation(render.LoadTexture("textures/enemy/poacherDie.png"), 80.0f, 200, false);

	currentAnimation = animations.shootLeft;

	drawRect = position * scale;
	hitBox = position;
	hitBox.x += 50;
	hitBox.y += 60;
	hitBox.z = 100;
	hitBox.w = 140;
	hitBox *= scale;
	modelMat = glmhelper::calcMatFromRect(drawRect, 0.0f, 1.0f);
}

void Poacher::setPoacher(glm::vec4 position)
{
	drawRect = position * scale;
	hitBox = position;
	hitBox.x += 40;
	hitBox.y += 130;
	hitBox.z = 125;
	hitBox.w = 100;
	hitBox *= scale;
	modelMat = glmhelper::calcMatFromRect(drawRect, 0.0f, 1.0f);
}

void Poacher::Update(Timer &timer, glm::vec4 mapRect, glm::vec2 playerPos, std::vector<Bullet> *bullets)
{
	if(gh::colliding(drawRect, mapRect))
		active = true;
	else
		active = false;
	if(active)
	{
		if(alive)
		{
			if(playerPos.x > drawRect.x + drawRect.z/2)
			{
				if(isLeft)
				{
					isLeft = false;
					auto old = currentAnimation;
					currentAnimation = animations.shootRight;
					currentAnimation.setFromOld(&old);
				}
			}
			else
			{
				if(!isLeft)
				{
					isLeft = true;
					auto old = currentAnimation;
					currentAnimation = animations.shootLeft;
					currentAnimation.setFromOld(&old);
				}
			}
			shootTimer += timer.FrameElapsed();
			currentFrame = currentAnimation.PingPongOnce(timer);

			if(currentAnimation.getIndex() == 3)
			{
				if(!shot)
				{
					shot = true;
					bullets->push_back(bullet);
					if(isLeft)
						bullets->back().modBullet(glm::vec2(-shootSpeed, 0.0f), glm::vec2(hitBox.x + hitBox.z/2, hitBox.y + hitBox.w/2 - 55.0f));
					else
						bullets->back().modBullet(glm::vec2(shootSpeed, 0.0f), glm::vec2(hitBox.x + hitBox.z/2, hitBox.y + hitBox.w/2 - 55.0f));
				}
			}
			else
			{
				shot = false;
			}

			shooting = currentAnimation.getIndex() != 0;
		
			if(shootTimer > shootDelay)
			{
				shootTimer = 0.0f;
				currentAnimation.Reset();
			}
		}
		else
		{
			currentFrame = currentAnimation.PlayOnce(timer);
		}
	}
}

void Poacher::kill()
{
	alive = false;
	currentAnimation = animations.dieLeft;
}

void Poacher::Draw(Render &render)
{ 
	if(active)
		render.DrawQuad(currentFrame.tex, modelMat, glm::vec4(1.0f), currentFrame.textureOffset);
	//render.DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(hitBox, 0, 5.0f), glm::vec4(1.0f));
}