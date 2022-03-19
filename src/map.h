#ifndef TILED_MAP_H
#define TILED_MAP_H

#include <string>
#include <vector>
#include <string>
#include <iostream>

#include "tiled.h"
#include <timer.h>
#include <glmhelper.h>
#include "vulkan-render/texture_loader.h"
#include "vulkan-render/texfont.h"
#include "vulkan-render/render.h"
#include "gamehelper.h"

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#endif

//#define SEE_COLLIDERS

struct Tile
{
	glm::vec4 tileRect;
	Resource::Texture texture;
};

struct MapText
{
	MapText(glm::vec4 rect, glm::vec4 colour, std::string text, float pixelSize)
	{
		this->rect = rect;
		this->colour = colour;
		this->text = text;
		this->pixelSize = pixelSize;
	}
	glm::vec4 rect;
	glm::vec4 colour;
	std::string text;
	float pixelSize;
	bool toDraw = false;
};

class Map
{
public:
Map() {}
Map(std::string filename, Render* render, float scale, Resource::Font* mapFont, float waterSpeed);
void Update(glm::vec4 cameraRect, Timer &timer);
void Draw(Render &render);
void Reset()
{
	if(switched)
		BlockSwitch();
	waterLevel = mapRect.w;
	waterTexLevel = waterLevel - map.tileHeight * 2.5;
	waterMove = 0.0f;
}
glm::vec4 getMapRect() { return mapRect; }
std::vector<glm::vec4> getColliders() 
{
	return currentFrameColliders; 
}
void BlockSwitch()
{
	switched = !switched;
	for(auto &s: switchBlocks)
	{
		s.active = !s.active;
		s.tileIndex += s.active ? -3 : 3;
	} 
}
std::vector<glm::vec4> getPoacherRects() { return poachers; }
std::vector<glm::vec4> getFruitRect() { return fruits; }
std::vector<glm::vec4> getCrabSpawns() { return crabs; }
glm::vec2 getPlayerSpawn() { return playerSpawn; };
glm::vec4 getGoal() { return goal; }
float getWaterLevel() { return waterLevel; }


private:

	struct TileDraw
	{
		TileDraw() {}
		TileDraw(Resource::Texture tex, glm::mat4 tileMat, glm::vec4 texOffset)
		{
			this->tex = tex;
			this->tileMat = tileMat;
			this->texOffset = texOffset;
		}
		Resource::Texture tex;
		glm::mat4 tileMat;
		glm::vec4 texOffset;
	};

	struct Switch
	{
		Switch(int index, glm::vec4 rect,glm::mat4 mat, bool active)
		{
			this->tileIndex = index;
			this->rect = rect;
			this->mat = mat;
			this->active = active;
		}
		int tileIndex;
		glm::vec4 rect;
		glm::mat4 mat;
		bool active = true;
	};

	tiled::Map map;
	std::vector<std::vector<glm::mat4>> tileMats;
	std::vector<MapText> mapTexts;
	std::vector<glm::vec4> tileRects;
	std::vector<glm::vec4> poachers;
	std::vector<glm::vec4> fruits;
	std::vector<glm::vec4> crabs;
	std::vector<Tile> tiles;
	//std::vector<glm::vec4> cameraRects;
	glm::vec4 mapRect;
	glm::vec4 goal;
	glm::vec2 playerSpawn;
	Resource::Font* mapFont;

	glm::vec4 lastCamRect;

	std::vector<glm::vec4> currentFrameColliders;
	std::vector<glm::vec4> colliders;
	std::vector<Switch> switchBlocks;

	std::vector<TileDraw> toDraw;


	Resource::Texture water;
	float waterLevel;
	float waterTexLevel;
	glm::mat4 waterMat;
	glm::mat4 bgWaterMat;
	glm::vec4 waterTexOffset;
	glm::vec4 bgWaterTexOffset;
	float waterRiseRate = 0.01f;
	float waterMoveRate = -0.05f;
	float waterMove = 0.0f;
	bool switched = false;
};


#endif