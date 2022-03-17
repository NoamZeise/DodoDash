#ifndef TILED_MAP_H
#define TILED_MAP_H

#include <string>
#include <vector>
#include <iostream>

#include "tiled.h"
#include <glmhelper.h>
#include "vulkan-render/texture_loader.h"
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

class Map
{
public:
Map() {}
Map(std::string filename, Render* render, float scale);
void Update(glm::vec4 cameraRect);
void Draw(Render &render);
glm::vec4 getMapRect() { return mapRect; }
std::vector<glm::vec4> getColliders() { return colliders; }
std::vector<glm::vec4> getPoacherRects() { return poachers; }
std::vector<glm::vec4> getFruitRect() { return fruits; }
glm::vec2 getPlayerSpawn() { return playerSpawn; };
glm::vec4 getGoal() { return goal; }


private:
	tiled::Map map;
	std::vector<std::vector<glm::mat4>> tileMats;
	std::vector<bool> toDraw;
	std::vector<glm::vec4> tileRects;
	std::vector<glm::vec4> poachers;
	std::vector<glm::vec4> fruits;
	std::vector<Tile> tiles;
	//std::vector<glm::vec4> cameraRects;
	glm::vec4 mapRect;
	glm::vec4 goal;
	glm::vec2 playerSpawn;

	std::vector<glm::vec4> colliders;

};


#endif