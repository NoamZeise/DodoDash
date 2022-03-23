#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

#include <string>

#include <timer.h>
#include <input.h>
#include <random.h>
#include <audio.h>
#include "vulkan-render/render.h"
#include "animation.h"
#include "vulkan-render/texture_loader.h"
#include <glmhelper.h>
#include "gamehelper.h"

#include "particle.h"

#include <iostream>

class Player
{
public:
	Player() {}
	Player(Render &render, float scale, ParticleManager* particleManager, Audio::Manager* manager);
	void Update(Timer &timer, Input &input, std::vector<glm::vec4> &colliders);
	void Draw(Render &render);
	glm::vec2 getMidPoint() { return glm::vec2(position.x + drawRect.z/2, position.y + drawRect.w/2); }
	glm::vec4 getHitBox() { return hitRect; }
	void bounce()
	{ 
		justBooseted = true;
		velocity.y = jumpMax;
		//yAcceleration = jumpAccel;		
		sinceJumpPressed = 0.0f;
		unpressedJump = true;
		isFloating = false;
		isBoosting = false;
		//grounded = true;
		//sinceGroundedTimer = 0.0f;
		//jumpTimer = 0.0f;
		boostTimer = 0.0f;
	}
	void Reset(glm::vec2 pos)
	{
		hp = maxHp;
		invincibilityTimer = invincibilityDelay;
		sinceJumpPressed = sinceJumpOld + 100.0f;
		unpressedJump = false;
		position = pos;
		layingEgg = false;
		EggDone = false;
	}
	bool isAlive() { return hp > 0; }
	void damage() 
	{ 
		if(invincibilityTimer > invincibilityDelay)
		{
			audioManager->Play("audio/sfx/HitSquawk" + std::to_string((int)(rand.PositiveReal()*5) + 1) + ".wav", false, 0.9f);
			justDamaged = true;
			invincibilityTimer = 0;
			hp--;
		}
	}
	void kill()
	{
		hp = 0;
	}
	void addHP(int i)
	{
		audioManager->Play("audio/sfx/EatFruit" + std::to_string((int)(rand.PositiveReal()*3) + 1) + ".ogg", false, 0.7f);
		if(hp + i > maxHp)
			hp = maxHp;
		else
			hp += i;
	}
	void setJumpPressed()
	{
		sinceJumpPressed = sinceJumpOld;
	}
	bool  isJumping() 
	{
		return jumping;
	}
	void EndLevel()
	{
		if(!layingEgg)
		{
			position.y += 40.0f;
			currentAnimation = animations.LayEgg;
			layingEgg = true;
		}
	}
	bool EggFinished()
	{
		return EggDone;
	}
	int getHp() { return hp; }
	int getHpMax() { return maxHp; }
	glm::vec2 getVelocity() { return this->velocity; }

private:
void controls(Timer &timer, Input &input);
void movement(Timer &timer, std::vector<glm::vec4> &colliders);
glm::vec4 colliding(std::vector<glm::vec4> &colliders, glm::vec4 checkRect);
void animate(Timer &timer);
void setAnimationPreserveFrame(Animation animation);

struct Animations
{
	Animation RunRight;
	Animation RunLeft;
	Animation JumpRight;
	Animation JumpLeft;
	Animation FlutterRight;
	Animation FlutterLeft;
	Animation LayEgg;
};
Animations animations;
Frame currentFrame;
Animation currentAnimation;

glm::vec2 position;
glm::vec2 prevPos;
glm::vec2 velocity;
glm::vec4 hitBoxOffset = glm::vec4(100, 110, 120, 160);
glm::vec4 hitRect;
float xAcceleration = 0.0025f;
float yAcceleration = 0.0f;
float xMax = 0.7f;
float xMaxNormal = 0.7f;
float xMaxPaniced = 0.9f;
float scale = 1.0f;
float friction = 0.8f;
float flyFloat = 0.4f;
float initalJumpVel = -0.02f;
float gravity = 0.008f;
float yMax = 1.2f;
float yMaximumFall = 1.0f;
float jumpAccel = -0.01f;
float jumpMax = -1.15f;

float jumpTimer = 0.0f;
float jumpDelay = 170.0f;

float boostTimer = 0.0f;
float boostDelay = 400.0f;
float boostRatio = 0.2f;

float sinceGroundedTimer = 0.0f;
float sinceGroundedDelay = 70.0f;

float sinceJumpPressed = 0.0f;
float sinceJumpOld = 100.0f;

bool invFlash = false;
bool justDamaged = false;
bool justBooseted = false;

float invincibilityDelay = 1500.0f;
float invincibilityTimer = invincibilityDelay;

bool grounded = false;
bool jumping = false;

int moveDir = 0;
int prevMoveDir = 0;
int hp = 2;
int maxHp = 2;
bool facingRight = true;

bool justJumped = false;
bool jumpPressed =false;
bool justFloat = false;

bool unpressedJump = false;
bool isFloating = false;
bool isBoosting = false;

float changeAmount = 0.2f;

bool layingEgg = false;
bool EggDone = false;

glm::vec4 drawRect;
glm::mat4 modelMat;

ParticleManager* particleManager;

float glideFeatherTimer = 0.0f;
float glideFeatherDelay = 100.0f;

float glideSoundTimer = 250.0f;
float glideSoundDelay = 250.0f;
bool playingGlide = false;

bool playedJump = false;
bool playedLand = false;
GameRandom rand;
Audio::Manager *audioManager;
};


#endif