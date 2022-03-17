#include "player.h"

Player::Player(Render &render, float scale)
{
	this->scale = scale;
	hitBoxOffset *= scale;
	Resource::Texture run = render.LoadTexture("textures/dodo/run.png");
	animations.RunRight = Animation(run, 50.0f, 200, false);
	animations.RunLeft = Animation(run, 50.0f, 200, true);
	Resource::Texture jump = render.LoadTexture("textures/dodo/jump.png");
	animations.JumpRight = Animation(jump, 60.0f, 200, false);
	animations.JumpLeft = Animation(jump, 60.0f, 200, true);
	Resource::Texture floatTex = render.LoadTexture("textures/dodo/float.png");
	animations.FloatRight = Animation(floatTex, 50.0f, 200, false);
	animations.FloatLeft = Animation(floatTex, 50.0f, 200, true);
	currentAnimation = animations.RunRight;
	position = glm::vec2(100, 300);
	velocity = glm::vec2(0, 0);
}

void Player::Update(Timer &timer, Input &input, std::vector<glm::vec4> &colliders)
{
	moveDir = 0;
	jumpPressed = false;
	if(input.Keys[GLFW_KEY_RIGHT])
	{
		moveDir += 1;
	}
	if(input.Keys[GLFW_KEY_LEFT])
	{
		moveDir -= 1;
	}
	if(sinceJumpPressed < 100.0f)
		unpressedJump = true;
	else
		unpressedJump = false;
	if(input.Keys[GLFW_KEY_Z])
	{
		sinceJumpPressed += timer.FrameElapsed();
		if(jumpTimer < jumpDelay)
		{
			if(sinceGroundedTimer < sinceGroundedDelay && unpressedJump)
			{
				velocity.y = initalJumpVel;
				yAcceleration = jumpAccel;
				sinceGroundedTimer = sinceGroundedDelay + 1;
			}
			unpressedJump = false;
		}
		else if( (unpressedJump || isFloating || isBoosting))
		{
			if(boostTimer < boostDelay)
			{
				if(!isBoosting)
				{
					velocity.y = velocity.y < 0 ? 0: velocity.y;
					isBoosting = true;
				}
				boostTimer += timer.FrameElapsed();
				if(velocity.y > 0)
					yAcceleration = jumpAccel;
				else
					yAcceleration = jumpAccel * boostRatio;
			}
			else
			{
				isFloating = true;
				auto sinceP = sinceJumpPressed - boostDelay;
				float floatPercent = sinceP < 1000.0f + boostDelay? sinceP/1000.0f : 1.0f;
				float floatFactor =  1.0f - (flyFloat * floatPercent);
				//std::cout << floatFactor << std::endl;
				yAcceleration = floatFactor * gravity;
				yMax = yMaximumFall * flyFloat;
			}
			unpressedJump = false;
		}
		else
		{
			yAcceleration = gravity;
			yMax = yMaximumFall;
		}
	}
	else
	{
		sinceJumpPressed = 0.0f;
		unpressedJump = true;
		isFloating = false;
		isBoosting = false;
		if(jumpTimer < jumpDelay && jumpTimer > 0)
		{
			jumpTimer = jumpDelay + 1;
		}
		yAcceleration = gravity;
		jumpTimer = jumpDelay + 1;
		yMax = yMaximumFall;
	}

	if((moveDir == -1 && velocity.x > 0) || (moveDir == 1 && velocity.x < 0))
	{
		velocity.x = 0;
	}
	velocity.x += moveDir * xAcceleration * timer.FrameElapsed();

	velocity.y += yAcceleration * timer.FrameElapsed();
	
	if(moveDir == 0)
		velocity.x *= friction;

	if(velocity.x > xMax)
		velocity.x = xMax;
	if(velocity.x < -xMax)
		velocity.x = -xMax;

	if(velocity.y > yMax)
		velocity.y = yMax;
	if(velocity.y < jumpMax)
		velocity.y = jumpMax;

	movement(timer, colliders);

	animate(timer);
	
	drawRect.x = position.x;
	drawRect.y = position.y;
	drawRect.z = currentFrame.size.x * scale;
	drawRect.w = currentFrame.size.y * scale;
	modelMat = glmhelper::calcMatFromRect(drawRect, 0.0f, 0.9f);
	prevMoveDir = moveDir;
	jumpTimer += timer.FrameElapsed();
	sinceGroundedTimer += timer.FrameElapsed();
	invincibilityTimer += timer.FrameElapsed();
}

void Player::movement(Timer &timer, std::vector<glm::vec4> &colliders)
{
	grounded = false;
	if(sinceGroundedTimer < sinceGroundedDelay)
	{
		grounded = true;
		jumpTimer = 0.0f;
		boostTimer = 0.0f;
	}
	position.x += velocity.x * timer.FrameElapsed();
	hitRect = glm::vec4(position.x + hitBoxOffset.x, position.y + hitBoxOffset.y, hitBoxOffset.z, hitBoxOffset.w);
	glm::vec4 rect = colliding(colliders, hitRect);
	if(rect != glm::vec4(0))
	{
		int change = 0;
		if(velocity.x > 0)
			change = -1;
		if(velocity.x < 0)
			change = 1;
		if(change != 0)
			while(gh::colliding(hitRect, rect))
			{
				position.x += change;
				hitRect.x += change;
			}
	}
	position.y += velocity.y * timer.FrameElapsed();
	hitRect = glm::vec4(position.x + hitBoxOffset.x, position.y + hitBoxOffset.y, hitBoxOffset.z, hitBoxOffset.w);
	rect = colliding(colliders, hitRect);
	if(rect != glm::vec4(0))
	{
		int change = 0;
		if(velocity.y > 0)
		{
			grounded = true;
			isFloating = false;
			isBoosting = false;
			sinceGroundedTimer = 0.0f;
			jumpTimer = 0.0f;
			boostTimer = 0.0f;
			velocity.y = 0.0f;
			change = -1;
		}
		if(velocity.y < 0)
		{
			velocity.y = 0;
			change = 1;
		}
		if(change != 0)
			while(gh::colliding(hitRect, rect))
			{
				position.y += change;
				hitRect.y += change;
			}
		position.y -= change/2;
		hitRect.y -= change/2;
	}
}

glm::vec4 Player::colliding(std::vector<glm::vec4> &colliders, glm::vec4 checkRect)
{
	for(const auto& rect: colliders)
	{
		if(gh::colliding(checkRect, rect))
		{
			return rect;
		}
	}
	return glm::vec4(0, 0, 0, 0);
}

void Player::animate(Timer &timer)
{
	if(jumpTimer > 0)
	{
		if(isFloating)
		{
			if(justFloat)
			{
				if(facingRight)
					currentAnimation = animations.FloatRight;
				else
					currentAnimation = animations.FloatLeft;

				justFloat = false;
				justJumped = true;
			}
			if(moveDir == 1 && prevMoveDir != 1)
			{
				setAnimationPreserveFrame(animations.FloatRight);
				facingRight = true;
			}
			else if(moveDir == -1 && prevMoveDir != -1)
			{
				setAnimationPreserveFrame(animations.FloatLeft);
				facingRight = false;
			}
		}
		else
		{
		if(justJumped)
		{
			if(facingRight)
				currentAnimation = animations.JumpRight;
			else
				currentAnimation = animations.JumpLeft;

			if(!jumpPressed || !justFloat)
				currentAnimation.setFrame(2);

			justJumped = false;
			justFloat = true;
			
		}
		if(moveDir == 1 && prevMoveDir != 1)
		{
			setAnimationPreserveFrame(animations.JumpRight);
			facingRight = true;
		}
		else if(moveDir == -1 && prevMoveDir != -1)
		{
			setAnimationPreserveFrame(animations.JumpLeft);
			facingRight = false;
		}
		}


		currentFrame = currentAnimation.PlayOnce(timer);
	}
	else
	{
		if(!justJumped || !justFloat)
		{
			justJumped = true;
			justFloat = true;
			if(facingRight)
				currentAnimation = animations.RunRight;
			else
				currentAnimation = animations.RunLeft;
		}
		if(moveDir == 1 && prevMoveDir != 1)
		{
			currentAnimation = animations.RunRight;
			facingRight = true;
		}
		if(moveDir == -1 && prevMoveDir != -1)
		{
			currentAnimation = animations.RunLeft;
			facingRight = false;
		}

		if(velocity.x > 0.05f || velocity.x < -0.05f)
		currentFrame = currentAnimation.Play(timer);
		else
		{
			currentAnimation.Reset();
			currentFrame = currentAnimation.getFrame(0);
		}
	}
}

void Player::Draw(Render &render)
{ 
	if(invincibilityTimer < invincibilityDelay)
		render.DrawQuad(currentFrame.tex, modelMat, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), currentFrame.textureOffset);
	else
		render.DrawQuad(currentFrame.tex, modelMat, glm::vec4(1.0f), currentFrame.textureOffset);
	//render.DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(hitRect, 0, 5.0f), glm::vec4(1.0f));
}

void Player::setAnimationPreserveFrame(Animation animation)
{
	int frame = currentAnimation.getIndex();
	currentAnimation = animation;
	currentAnimation.setFrame(frame);
}
