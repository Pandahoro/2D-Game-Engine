#include "Map.h"

DPhoenix::Map::Map(std::string filename, TextureMgr* mTexMgr, ID3D11Device* md3dDevice,
	std::vector<Collectable*>& collectablesVec, std::vector<std::vector<XMFLOAT2*>>& enemySpawnVec,
	int tileWidthPerPanel, int tileHeightPerPanel, int amtPanelsWide, int amtPanelsHigh)
{
	//set map position (map begins at (0,0))
	mPosition.x = 0.0f;
	mPosition.y = 0.0f;
	//initialise player spawn point (overwritten by map data)
	mPlayerSpawnPoint.x = 0.0f; mPlayerSpawnPoint.y = 0.0f;

	//get from CSV file and populate 2D vector
	std::ifstream inMapFile(filename);

	//helpers for parsing CSV file
	std::string value;
	std::string ignore;
	//caluclate the width and height in tiles
	mWidth = tileWidthPerPanel * amtPanelsWide;
	mHeight = tileHeightPerPanel * amtPanelsHigh;
	//size of tiles (assumed square)
	mTileSize = 30;

	//values used in constructing map
	int row = mHeight - 1; //counting backwards because it stacks tiles
	int col = 0;
	int count = 0;

	//set the vector values based on our tile sizes
	//(build 2D array for tiles)
	mTiles.resize(mWidth);
	for (int i = 0; i < mTiles.size(); i++)
	{
		mTiles[i].resize(mHeight);
	}
	//used when setting collectable positions on map loading
	XMFLOAT2 collectablePos;

	//size the enemy spawn vector so it has vectors for each enemy type
	enemySpawnVec.resize(ENEMYMAP_MAX);
	XMFLOAT2 objectPos; //used to set enemy positions

	//map parsing; starting at the top row and going by column
	//after each row has the column value filled, decrement (go down a row)
	//and repeat action - builds map array as it appears in the spreadsheet

	//read in map file (CSV) and continue while open
	while (inMapFile.good())
	{
		//get line until next comma - store in value
		std::getline(inMapFile, value, '\n');

		//if we have an actual line
		if (value.length() > 1)
		{
			//loop through the chars in the value given
			for (int i = 0; i < value.length(); i++)
			{
				//we'll be using this portion of the ascii table
				//which doesn't include commas or file header weirdness
				//hence we can check we are between these values
				int indexStart = (int)'0';
				int indexEnd = (int)'z';
				//if the char value is within the ascii table values set
				if ((int)value[i] >= indexStart && (int)value[i] <= indexEnd)
				{
					//check what the value is for this tile
					switch (value[i])
					{
					case '0':
						mTiles[col][row] = EMPTY_TILE;
						break;
					case '1':
						mTiles[col][row] = BLOCK_TILE;
						break;
					case '2':
						mTiles[col][row] = ONEWAY_TILE;
						break;
					case 'd':
						mPlayerSpawnPoint.x = mPosition.x + col * mTileSize;
						mPlayerSpawnPoint.y = mPosition.y + row * mTileSize;
						break;
					case 'c':
						//get position to place collectable based on map position
						collectablePos.x = mPosition.x + col * mTileSize;
						collectablePos.y = mPosition.y + row * mTileSize;
						//construct new collectable to update vector
						collectablesVec.push_back(new Collectable(mTexMgr, md3dDevice, 
							collectablePos, COLLECTABLE_COFFEE));

						break;
					case 'r':
						//get position to place collectable based on map position
						collectablePos.x = mPosition.x + col * mTileSize;
						collectablePos.y = mPosition.y + row * mTileSize;
						//construct new collectable to update vector
						collectablesVec.push_back(new Collectable(mTexMgr, md3dDevice, 
							collectablePos, COLLECTABLE_RESEARCH));
						break;
					case 'p': //chemtrails plane enemy
						//set position to set enemy spawn point based on map position
						objectPos.x = mPosition.x + col * mTileSize;
						objectPos.y = mPosition.y + row * mTileSize;
						//add the new spawn point to the vector indexed for chemplane type
						enemySpawnVec[ENEMYMAP_CHEMPLANE].push_back(new XMFLOAT2(objectPos));
						break;
					case 'v':	//antivaxxer
						//set position to set enemy spawn point based on map position
						objectPos.x = mPosition.x + col * mTileSize;
						objectPos.y = mPosition.y + row * mTileSize;
						//add the new spawn point to the vector indexed for antivax type
						enemySpawnVec[ENEMYMAP_ANTIVAX].push_back(new XMFLOAT2(objectPos));
						break;
					}
					//increment column
					col++;
				}
			}
			//decrement row; tile rows stack on top of each other
			row--; col = 0;
		}
	}

	//need also Sprites for tiles to render
	mOneWayTileSprite = new DPhoenix::Sprite();
	mOneWayTileSprite->Load("Textures\\LevelAssets\\OneWayTile.png", mTexMgr, 30.0f, 30.0f, md3dDevice);
	mOneWayTileSprite->SetCurrentFrame(0);	mOneWayTileSprite->mAnimationColumns = 1;
	mOneWayTileSprite->mAnimationDirection = 0;	mOneWayTileSprite->SetAnimationRange(0, 0);
	mOneWayTileSprite->SetScale(1.0f);	mOneWayTileSprite->mFlipValue = 1.0f;

	mSolidTileSprite = new DPhoenix::Sprite();
	mSolidTileSprite->Load("Textures\\LevelAssets\\SolidTile.png", mTexMgr, 30.0f, 30.0f, md3dDevice);
	mSolidTileSprite->SetCurrentFrame(0);	mSolidTileSprite->mAnimationColumns = 1;
	mSolidTileSprite->mAnimationDirection = 0;	mSolidTileSprite->SetAnimationRange(0, 0);
	mSolidTileSprite->SetScale(1.0f);	mSolidTileSprite->mFlipValue = 1.0f;

}

//given a point, what tile ([x][y] array position) is at this point
XMFLOAT2 DPhoenix::Map::GetMapTileAtPoint(XMFLOAT2 point)
{
	int x;
	int y;

	//offset the map tile at the point given from the map position
	//we divide the tilesize by 2 because we're looking for the centre point
	//the final division by tilesize is so we get a corresponding tile index
	x = (int)((point.x - mPosition.x + mTileSize / 2.0f) / (float)mTileSize);
	y = (int)((point.y - mPosition.y + mTileSize / 2.0f) / (float)mTileSize);

	return XMFLOAT2(x, y);
}

//as above, but y index
int DPhoenix::Map::GetMapTileYAtPoint(float y)
{
	return (int)((y - mPosition.y + mTileSize / 2.0f) / (float)mTileSize);
}

//as above but x index
int DPhoenix::Map::GetMapTileXAtPoint(float x)
{
	return (int)((x - mPosition.x + mTileSize / 2.0f) / (float)mTileSize);
}

//get the actual position (x,y) of a map tile given the [x][y] array position
XMFLOAT2 DPhoenix::Map::GetMapTilePosition(int tileIndexX, int tileIndexY)
{
	//multiply by tilesize and offset by map position
	return XMFLOAT2(
		(float)(tileIndexX * mTileSize) + mPosition.x,
		(float)(tileIndexY * mTileSize) + mPosition.y
	);
}
//as above but takes in XMFLOAT2
XMFLOAT2 DPhoenix::Map::GetMapTilePosition(XMFLOAT2 tileCoords)
{
	return XMFLOAT2(
		(float)(tileCoords.x * mTileSize) + mPosition.x,
		(float)(tileCoords.y * mTileSize) + mPosition.y
	);
}

//this gets the tile type given the [x][y] array position
DPhoenix::TileTypes DPhoenix::Map::GetTile(int x, int y)
{
	//if at edge of level, block it off
	if (x < 0 || x >= mWidth
		|| y < 0 || y >= mHeight)
		return BLOCK_TILE;
	//return the tile point at given indices
	return mTiles[x][y];
}

//given the [x][y] array position, is the tile type
//an obstacle / solid?
bool DPhoenix::Map::IsObstacle(int x, int y)
{
	//treat edge of map as solid
	if (x < 0 || x >= mWidth
		|| y < 0 || y >= mHeight)
		return true;

	return  (mTiles[x][y] == BLOCK_TILE);
}

//given the [x][y] array position, is the tile type
//a ground tile?
bool DPhoenix::Map::IsGround(int x, int y)
{
	if (x < 0 || x >= mWidth
		|| y < 0 || y >= mHeight)
		return false;

	return (mTiles[x][y] == ONEWAY_TILE || mTiles[x][y] == BLOCK_TILE);
}

//given the [x][y] array position, is the tile type
//a one way tile?
bool DPhoenix::Map::IsOneWayPlatform(int x, int y)
{
	if (x < 0 || x >= mWidth
		|| y < 0 || y >= mHeight)
		return false;

	return (mTiles[x][y] == ONEWAY_TILE);
}

//given the [x][y] array position, is the tile type
//an empty tile?
bool DPhoenix::Map::IsEmpty(int x, int y)
{
	if (x < 0 || x >= mWidth
		|| y < 0 || y >= mHeight)
		return false;

	return (mTiles[x][y] == EMPTY_TILE);
}

