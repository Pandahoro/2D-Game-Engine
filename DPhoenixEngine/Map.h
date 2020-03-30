#pragma once
#include "D3DUtil.h"
#include "Collectable.h"

namespace PandaEngine
{
	//sort of duplication but prevents circular linking
	//the things we do to avoid globals eh?
	enum EnemyTypesForMap
	{
		ENEMYMAP_CHEMPLANE,
		ENEMYMAP_ANTIVAX,
		ENEMYMAP_MAX
	};

	//types of tiles for rendering / collision
	enum TileTypes
	{
		EMPTY_TILE,
		BLOCK_TILE,
		ONEWAY_TILE
	};

	class Map
	{

	public:
		//using vectors as opposed to arrays 
		//simply so they can be dynamically resized without
		//resorting to globals
		std::vector<std::vector<TileTypes>> mTiles;
		//we also need something to hold the visible sprites
		Sprite* mOneWayTileSprite;
		Sprite* mSolidTileSprite;
		Sprite* mCollidedTileSprite;

		//map position
		XMFLOAT3 mPosition;
		//width and height in tiles
		int mWidth;
		int mHeight;

		//player spawn point / position
		XMFLOAT2 mPlayerSpawnPoint;

		//tilesize
		int mTileSize;

		//constructor - takes in map data filename, texture manager, d3d device
		//and amount of tiles per panel and amount of panels (rows and columns)
		//the enemies spawn vector is 2D (vector of vectors of XMFLOAT2)
		//that way we can check how many enemies of each type are spawning in our main demo
		Map(std::string filename, TextureMgr* mTexMgr, ID3D11Device* md3dDevice,
			std::vector<Collectable*>& collectablesVec, 
			std::vector<std::vector<XMFLOAT2*>>& enemySpawnVec,
			int tileWidthPerPanel, int tileHeightPerPanel, 
			int amtPanelsWide, int amtPanelsHigh);
		~Map() { };

		//helper methods to determine tiles
		//given a point, what tile ([x][y] array position) is at this point
		XMFLOAT2 GetMapTileAtPoint(XMFLOAT2 point);
		//get the [y] array position at point given
		int GetMapTileYAtPoint(float y);
		//get the [x] array position at point given
		int GetMapTileXAtPoint(float x);

		//these get the actual position (x,y) of a map tile given the [x][y] array position
		XMFLOAT2 GetMapTilePosition(int tileIndexX, int tileIndexY);
		XMFLOAT2 GetMapTilePosition(XMFLOAT2 tileCoords);
		//this gets the tile type given the [x][y] array position
		TileTypes GetTile(int x, int y);

		//these are checks to see what type of tile is at
		//a given array position point [x][y]
		bool IsObstacle(int x, int y);
		bool IsGround(int x, int y);
		bool IsOneWayPlatform(int x, int y);
		bool IsEmpty(int x, int y);

	};
}

