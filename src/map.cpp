#include "map.h"

Map::Map(std::string filename, Render* render, float scale, Resource::Font* mapFont, float waterSpeed)
{
	this->mapFont = mapFont;
	map = tiled::Map(filename);
	map.tileWidth *= scale;
	map.tileHeight *= scale;
	mapRect = glm::vec4(0, 0, map.width * map.tileWidth, map.height * map.tileHeight);

	this->waterRiseRate = waterSpeed;
	this->waterMoveRate = -waterSpeed * 5;
	water = render->LoadTexture("textures/water.png");
	Reset();

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
			index++;
		}
		

	int smallestSwitch = 0;
	int layerIndex = -1;
	for(const auto &layer: map.layers)
	{
		layerIndex++;
		if(layer.props.collidable  || layer.props.switching)
		{
			size_t i = 0;
			for(unsigned int y = 0; y < map.height; y++)
				for(unsigned int x = 0; x < map.width; x++)
				{
					if(layer.data[i] != 0 && layer.props.collidable)
						colliders.push_back(glm::vec4(x * map.tileWidth, y * map.tileHeight, map.tileWidth, map.tileHeight));
					if(layer.data[i] != 0 && layer.props.switching)
					{
						if(smallestSwitch == 0 || layer.data[i] < smallestSwitch)
							smallestSwitch = layer.data[i];
						switchBlocks.push_back(
							Switch(layer.data[i],
							glm::vec4(x * map.tileWidth, y * map.tileHeight, map.tileWidth, map.tileHeight),
							tileMats[layerIndex][i],
							false));
					}
					i++;
				}
		}
	}

	for (auto &s: switchBlocks)
	{
		if(s.tileIndex < smallestSwitch + 3)
			s.active = true;
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
			if(obj.props.crab || objGroup.props.crab)
				crabs.push_back(glm::vec4(obj.x, obj.y, obj.w, obj.h));
		}
	}
	
	for(const auto &txt: map.texts)
	{
		mapTexts.push_back(
			MapText(
				glm::vec4(txt.obj.x * scale, txt.obj.y * scale, txt.obj.w * scale * 1.2f, txt.obj.h * scale),
				glm::vec4(txt.colour.r / 255.0f, txt.colour.g / 255.0f, txt.colour.b / 255.0f, txt.colour.a / 255.0f),
				txt.text, 
				txt.pixelSize * scale));
	}

	colliders.push_back(glm::vec4(-100, -100, 100, mapRect.w + 100));
	colliders.push_back(glm::vec4(mapRect.z, -100, 100, mapRect.w + 100));
}


void Map::Update(glm::vec4 cameraRect, Timer &timer)
{
	lastCamRect = cameraRect;

	waterLevel -= timer.FrameElapsed() * waterRiseRate;
	waterTexLevel -= timer.FrameElapsed() * waterRiseRate;
	waterMove += timer.FrameElapsed() * waterMoveRate;
	if(waterMove > mapRect.z)
		waterMove = 0.0f;
	waterTexOffset = glmhelper::calcTexOffset(water.dim, glm::vec4(mapRect.x + waterMove, mapRect.y, mapRect.z, water.dim.y));

	waterMat = glmhelper::calcMatFromRect(glm::vec4(0, waterTexLevel, mapRect.z, water.dim.y), 0.0f, 2.0);

	for(auto &txt: mapTexts)
	{
		txt.toDraw = gh::colliding(txt.rect, cameraRect);
	}
	toDraw.clear();
	for(unsigned int tile = 0; tile < tileRects.size(); tile++)
		if(gh::colliding(cameraRect, tileRects[tile]))
			for(unsigned int layer = 0; layer < map.layers.size(); layer++)
				if(!map.layers[layer].props.switching && map.layers[layer].data[tile] != 0)
					toDraw.push_back(TileDraw(tiles[map.layers[layer].data[tile]].texture, tileMats[layer][tile], tiles[map.layers[layer].data[tile]].tileRect));
	
}

void Map::Draw(Render &render)
{
	#ifdef SEE_COLLIDERS
	for(const auto &rect: colliders)
	{
		render.DrawQuad(Resource::Texture(), glmhelper::calcMatFromRect(rect, 0, 5.0f), glm::vec4(1.0f));
	}
	#endif
	for(auto &txt: mapTexts)
	{
		if(txt.toDraw)
			render.DrawString(mapFont, txt.text,
				glm::vec2(txt.rect.x, txt.rect.y),
				 txt.pixelSize, 0, txt.colour, 0.0f);
	}

	for(unsigned int i = 0; i < toDraw.size(); i++)
	{
		render.DrawQuad(toDraw[i].tex, toDraw[i].tileMat, glm::vec4(1.0f), toDraw[i].texOffset);
	}

	for(unsigned int i = 0; i < switchBlocks.size(); i++)
	{
		render.DrawQuad(tiles[switchBlocks[i].tileIndex].texture, switchBlocks[i].mat, glm::vec4(1.0f), tiles[switchBlocks[i].tileIndex].tileRect);
	}

	render.DrawQuad(water, waterMat, glm::vec4(1.0f, 1.0f, 1.0f, 0.9f), waterTexOffset);
}