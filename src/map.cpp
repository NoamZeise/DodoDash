#include "map.h"

Map::Map(std::string filename, Render* render, float scale)
{
	map = tiled::Map(filename);
	map.tileWidth *= scale;
	map.tileHeight *= scale;
	mapRect = glm::vec4(0, 0, map.width * map.tileWidth, map.height * map.tileHeight);

	tileMats.resize(map.layers.size());
	toDraw.resize(map.width * map.height);
	tileRects.resize(map.width * map.height);
	int index = 0;
	for(unsigned int y = 0; y < map.height; y++)
		for(unsigned int x = 0; x < map.width; x++)
		{
			tileRects[index] = glm::vec4(x * map.tileWidth, y * map.tileHeight, map.tileWidth, map.tileHeight);
			for(unsigned int i = 0; i < map.layers.size(); i++) 
				tileMats[i].push_back(glmhelper::calcMatFromRect(tileRects[index], 0, 0.0f + (float)i/10.0f));
			toDraw[index++] = false;
		}
		

	for(const auto &layer: map.layers)
	{
		if(layer.props.collidable)
		{
			size_t i = 0;
			for(unsigned int y = 0; y < map.height; y++)
				for(unsigned int x = 0; x < map.width; x++)
				{
					if(layer.data[i] != 0 && layer.props.collidable)
						colliders.push_back(glm::vec4(x * map.tileWidth, y * map.tileHeight, map.tileWidth, map.tileHeight));
					i++;
				}
		}
	}

	tiles.resize(map.totalTiles + 1);
	tiles[0] = Tile();
	tiles[0].tileRect = glm::vec4(0, 0, 1, 1);
	for(const auto &tileset: map.tilesets)
	{
		Resource::Texture tex = render->LoadTexture(tileset.imageSource);
		unsigned int id = tileset.firstTileID;
		for(unsigned int y = 0; y < tileset.imageHeight / tileset.tileHeight; y++)
			for(unsigned int x = 0; x < tileset.columns; x++)
			{
				tiles[id] = Tile();
				tiles[id].texture = tex;
				tiles[id++].tileRect = glmhelper::calcTexOffset(glm::vec2(tileset.imageWidth, tileset.imageHeight),
					glm::vec4(x * tileset.tileWidth, y * tileset.tileHeight, tileset.tileWidth, tileset.tileHeight));
			}
	}

	for(const auto &objGroup: map.objectGroups)
	{
		for(const auto &obj: objGroup.objs)
		{
			if(obj.props.collidable || objGroup.props.collidable)
				colliders.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));
			if(obj.props.poacher || objGroup.props.poacher)
				poachers.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));
			if(obj.props.playerSpawn || objGroup.props.playerSpawn)
				playerSpawn = glm::vec2(obj.x, obj.y);
			if(obj.props.goal || objGroup.props.goal)
				goal = glm::vec4(obj.x, obj.y, obj.w, obj.h);
			if(obj.props.fruit || objGroup.props.fruit)
				fruits.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));
		}
	}
	colliders.push_back(glm::vec4(-100, -100, 100, mapRect.w + 100));
	colliders.push_back(glm::vec4(mapRect.z, -100, 100, mapRect.w + 100));
}


void Map::Update(glm::vec4 cameraRect)
{
	for(unsigned int i = 0; i < toDraw.size(); i++)
	{
		if(gh::colliding(cameraRect, tileRects[i]))
			toDraw[i] = true;
		else
			toDraw[i] = false;
	}
}

void Map::Draw(Render &render)
{
	#ifdef SEE_COLLIDERS
	for(const auto &rect: colliders)
	{
		render.DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(rect, 0, 5.0f), glm::vec4(1.0f));
	}
	#endif
	for(unsigned int i = 0; i < map.layers.size(); i++)
	{
		for(unsigned int j = 0; j < map.layers[i].data.size(); j++)
		{
			if(toDraw[j])
				if(map.layers[i].data[j] != 0)
					render.DrawQuad(tiles[map.layers[i].data[j]].texture, tileMats[i][j], glm::vec4(1.0f), tiles[map.layers[i].data[j]].tileRect);
		}
	}
}