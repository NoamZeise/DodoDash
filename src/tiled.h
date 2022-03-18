#ifndef TILED_MAP_PARSER_H
#define TILED_MAP_PARSER_H

#include <rapidxml.hpp>

#include <string>
#include <fstream>
#include <stdexcept>
#include <stdlib.h>
#include <vector>
#include <iostream>

namespace tiled
{


//CODE THAT YOU NEED TO CHANGE TO MATCH YOUR SETUP

const std::string TILEMAP_LOCATION = "maps/";
const std::string TILED_TEXTURE_LOCATION = "textures/tilesets";
const std::string TILED_IMAGE_LOCATION = "textures/tiledimage";

//some example properties -> match what you set in tiled (do not need to be boolean, but you need to change the fillPropStruct to match those cases)
struct Properties 
{
	bool collidable = false;
	bool camera = false;
	bool playerSpawn = false;
	bool enemySpawn = false;
	bool poacher = false;
	bool goal = false;
	bool fruit = false;
};

//Match names and types of properties set in Tiled
Properties fillPropStruct(rapidxml::xml_node<> *propertiesNode);

static char* loadTextFile(std::string filename);

struct Layer
{
	Properties props;
	std::vector<unsigned int> data;
};

struct Object
{
	Properties props;
	double x = 0;
	double y = 0;
	double w = 0;
	double h = 0;
};

struct ObjectGroup
{
	Properties props;
	std::vector<Object> objs;
};

struct ImageLayer
{
	std::string source;
	double x = 0;
	double y = 0;
};

struct Tileset
{
	Tileset(std::string filename);

	unsigned int firstTileID = 0;

	std::string name;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int tileCount;
	unsigned int columns;

	std::string imageSource;
	unsigned int imageWidth;
	unsigned int imageHeight;
};

struct Colour
{
	Colour() { r = 0; g = 0; b = 0; a = 0;}
	Colour(int r, int g, int b, int a) { this->r = r; this->g = g; this->b = b; this->a = a; }
	int r;
	int g;
	int b;
	int a;
};

struct Text
{
	Object obj;
	Colour colour;
	std::string text;
	int pixelSize;
	int wrap;
};

struct Map
{
public:
	Map(std::string filename);
	Map() {}

	unsigned int width;
	unsigned int height;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int totalTiles;

	std::vector<Tileset> tilesets;
	std::vector<Layer> layers;
	std::vector<ObjectGroup> objectGroups;
	std::vector<ImageLayer> imgLayer;
	std::vector<Text> texts;
};

} //namespace end

#endif