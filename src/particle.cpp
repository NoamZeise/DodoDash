#include "particle.h"


Particle::Particle(Resource::Texture texture, glm::vec4 rect, glm::vec2 velocity, float lifespan, glm::vec4 initialColour, glm::vec4 finalColour, float rotation, float slow, float dist)
{
	this->texture = texture;
	this->rect = rect;
	this->velocity = velocity;
	this->lifespan = lifespan;
	this->initialColour = initialColour;
	this->finalColour = finalColour;
	this->rotation = rotation;
	this->slow = slow;
	this->dist = dist;
}

void Particle::Update(Timer &timer, glm::vec4 camRect, glm::vec4 deltaRect)
{
	lifetime += timer.FrameElapsed();
	rect.x += currentVelocity.x * timer.FrameElapsed();
	rect.x += deltaRect.x * dist;
	rect.y += currentVelocity.y * timer.FrameElapsed();

	glm::vec4 frameRect = glm::vec4(rect.x, rect.y, rect.z, rect.w);
	active = gh::colliding(frameRect, camRect);
	if(active)
	{
		modelMat = glmhelper::calcMatFromRect(frameRect, rotation, 2.0f);
		auto ratio = lifetime / lifespan;
		auto inverseRatio = 1 - ratio;
		currentColour = glm::vec4((initialColour.x * inverseRatio) + (finalColour.x * ratio),
			(initialColour.y * inverseRatio) + (finalColour.y * ratio),
			(initialColour.z * inverseRatio) + (finalColour.z * ratio),
			(initialColour.w * inverseRatio) + (finalColour.w * ratio));
		currentVelocity = (velocity * inverseRatio) + ((velocity*slow) * ratio);;
	}
}

void Particle::Draw(Render &render)
{
	if(active)
		render.DrawQuad(texture, modelMat, currentColour);
}

ParticleManager::ParticleManager(Render &render)
{
	this->featherTex = render.LoadTexture("textures/feather.png");
	this->rainTex = render.LoadTexture("textures/raindrop.png");
}

void ParticleManager::Update(Timer &timer, glm::vec4 camRect)
{
	rainTimer += timer.FrameElapsed();
	if(rainTimer > rainDelay)
	{
		rainTimer = 0.0f;
		for(int i = 0; i < rainCount; i++)
		{
			float rainMod = rand.PositiveReal();
			EmitRain(glm::vec2(camRect.x + ((settings::TARGET_WIDTH + 300)*rand.PositiveReal()), camRect.y - 10));
		}
	}
	glm::vec4 deltaCam = camRect;
	deltaCam.x -= prevCam.x;
	deltaCam.y -= prevCam.y;
	for(int i = 0; i < particles.size(); i++)
	{
		particles[i].Update(timer, camRect, deltaCam);
		if(particles[i].isRemoved())
			particles.erase(particles.begin() + i--);
	}
	prevCam = camRect;
}

void ParticleManager::Draw(Render &render)
{
	for(auto &ptcle: particles)
		ptcle.Draw(render);
}

void ParticleManager::EmitFeather(glm::vec2 pos, glm::vec2 playerVelocity, float lifetime)
{
	playerVelocity.x = (playerVelocity.x * rand.PositiveReal()*0.25f) + rand.Real()*0.5f;
	playerVelocity.y = (playerVelocity.x * rand.PositiveReal()*0.25f) + rand.Real()*0.5f;
	float rot = rand.Real() * 360.0f;
	//lifetime += (200.0f * rand.PositiveReal());
	particles.push_back(
		Particle(
			featherTex, 
			glm::vec4(pos.x, pos.y, featherTex.dim.x/1.5f, featherTex.dim.y/1.5f),
			-playerVelocity,
			lifetime,
			glm::vec4(1, 1, 1, 1),
			glm::vec4(1, 1, 1, 0),
			rot,
			0.25f,
			0.0f
		));
}

void ParticleManager::EmitRain(glm::vec2 pos)
{
	//float rot = rand.Real() * 360.0f;
	//lifetime += (200.0f * rand.PositiveReal());
	float dist = rand.PositiveReal();
	//pos.x *= dist;

	particles.push_back(
		Particle(
			rainTex, 
			glm::vec4(pos.x, pos.y, rainTex.dim.x * 0.15f, rainTex.dim.y * 0.25f),
			glm::vec2(0.0f, 1.5f + rand.Real()*0.4f),
			900.0f,
			glm::vec4(1, 1, 1, 1.0f) * (rand.PositiveReal() + 0.5f),
			glm::vec4(1, 1, 1, 1.0f) * (rand.PositiveReal() + 0.5f),
			10.0f,
			0.9f,
			dist
		));
}
