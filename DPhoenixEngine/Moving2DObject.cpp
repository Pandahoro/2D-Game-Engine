#include "Moving2DObject.h"

void DPhoenix::Moving2DObject::UpdatePhysics(float deltaTime, DPhoenix::Map* map)
{
	//hold old values before calculations
	mOldPosition = mPosition;
	mOldSpeed = mSpeed;

	//hold old values before new flags set
	mOnGroundJust = mOnGroundIs;
	mPushRightWallJust = mPushRightWallIs;
	mPushLeftWallJust = mPushLeftWallIs;
	mAtCeilingJust = mAtCeilingIs;

	//position adjusted by speed (adjusted by time difference)
	mPosition.x += mSpeed.x * deltaTime;
	mPosition.y += mSpeed.y * deltaTime;

	//left
	float leftWallX = 0;

	if (mSpeed.x <= 0.0f
		&& CollidesWithLeftWall(mOldPosition, mPosition, &leftWallX, map)
		&& mIsTileCollider && mIsWallCollider)
	{
		mPosition.x = leftWallX + mAABB.halfSize.x - mAABBOffset.x;
		mPushLeftWallIs = true;
		mSpeed.x = max(mSpeed.x, 0.0f);
	}
	else
		mPushLeftWallIs = false;

	//right
	float rightWallX = 0;

	if (mSpeed.x >= 0.0f
		&& CollidesWithRightWall(mOldPosition, mPosition, &rightWallX, map)
		&& mIsTileCollider && mIsWallCollider)
	{
		mPosition.x = rightWallX - mAABB.halfSize.x - mAABBOffset.x;
		mPushRightWallIs = true;
		mSpeed.x = min(mSpeed.x, 0.0f);
	}
	else
		mPushRightWallIs = false;

	//reset flag before checks
	mOnOnewayPlatformIs = false;
		
	float groundY = 0;
	//are we moving negatively? check on ground wiuth tiles
	if (mSpeed.y <= 0.0f
		&& HasGround(mOldPosition, mPosition, mSpeed, &groundY, map)
		&& mIsTileCollider)
	{
		//if so we adjust our Y position based on the groiund and our AABB
		mPosition.y = groundY + mAABB.halfSize.y - mAABBOffset.y;
		mSpeed.y = 0.0f;
		mOnGroundIs = true;
	}
	else
		mOnGroundIs = false;

	//now for the ceiling
	float ceilingY = 0;
	//are we moving positively? check with ceiling tiles
	if (mSpeed.y >= 0.0f
		&& HasCeiling(mOldPosition, mPosition, mSpeed, &ceilingY, map)
		&& mIsTileCollider)
	{
		//if so we adjust our Y position based on the ground and our AABB
		mPosition.y = ceilingY - mAABB.halfSize.y - mAABBOffset.y - 1.0f;
		mSpeed.y = 0.0f;
		mAtCeilingIs = true;
	}
	else
		mAtCeilingIs = false;	

	//update bounding box centre by offset
	mAABB.center.x = mPosition.x + mAABBOffset.x;
	mAABB.center.y = mPosition.y + mAABBOffset.y;

}

bool DPhoenix::Moving2DObject::HasGround(XMFLOAT2 oldPosition, XMFLOAT2 position,
	XMFLOAT2 speed, float* groundY, DPhoenix::Map* map)
{
	//old positions calculated too in case we are falling too fast (bullet through paper)
	XMFLOAT2 oldCenter(oldPosition.x + mAABBOffset.x, oldPosition.y + mAABBOffset.y);
	XMFLOAT2 center(position.x + mAABBOffset.x, position.y + mAABBOffset.y);

	//calculate 1px high AABB at bottom of object's AABB - old position and new
	XMFLOAT2 oldBottomLeft(oldCenter.x - mAABB.halfSize.x + 1.0f, oldCenter.y - mAABB.halfSize.y - 1.0f);
	XMFLOAT2 newBottomLeft(center.x - mAABB.halfSize.x + 1.0f, center.y - mAABB.halfSize.y - 1.0f);
	XMFLOAT2 newBottomRight(newBottomLeft.x + mAABB.halfSize.x * 2.0f - 2.0f, newBottomLeft.y);

	//We start the search from the tile at the previous frame's sensor position, 
	//and end it at the current frame's sensor position. That's of course because 
	//when we check for a ground collision we assume we are falling down, and that 
	//means we're moving from the higher position to the lower one.
	int endY = map->GetMapTileYAtPoint(newBottomLeft.y);
	int begY = max(map->GetMapTileYAtPoint(oldBottomLeft.y) - 1, endY);
	//calculate distance between - max value should be 1
	int dist = max(abs(endY - begY), 1);
	//var declaration to use in checking
	int tileIndexX;

	//we start at the old position then check tiles up to the new position
	for (int tileIndexY = begY; tileIndexY >= endY; --tileIndexY)
	{
		//interpolate between positions based on distance to get a bottom left
		//value to check against
		XMVECTOR bottomLeftVec = XMVectorLerp(XMLoadFloat2(&newBottomLeft),
			XMLoadFloat2(&oldBottomLeft),
			(float)abs(endY - tileIndexY) / dist);
		//number conversions using XNA maths
		XMFLOAT2 bottomLeft;
		XMStoreFloat2(&bottomLeft, bottomLeftVec);
		//and now for the right corner
		XMFLOAT2 bottomRight(bottomLeft.x + mAABB.halfSize.x * 2.0f - 2.0f, bottomLeft.y);

		//here we check the ground collision along the width of the bottom sensor AABB
		for (XMFLOAT2 checkedTile = bottomLeft; ; checkedTile.x += map->mTileSize)
		{
			//make sure we're not moving too far out to the right
			checkedTile.x = min(checkedTile.x, bottomRight.x);

			//get the tile index
			tileIndexX = map->GetMapTileXAtPoint(checkedTile.x);

			//calculate tile's top position
			//note the dereference operator (*) so we set the value rather than overwrite pointer
			*groundY = (float)tileIndexY * map->mTileSize + map->mTileSize / 2.0f + map->mPosition.y;

			//check if obstacle / OR one way platform
			if (map->IsObstacle(tileIndexX, tileIndexY))
			{
				mOnOnewayPlatformIs = false;
				return true;
			}
			//AND we haven't gone beyond the floor sensor threshold between frames
			else if (map->IsOneWayPlatform(tileIndexX, tileIndexY)
				&& abs(checkedTile.y - *groundY) <= mOneWayPlatformThreshold + 
				mOldPosition.y - position.y)
				mOnOnewayPlatformIs = true;

			//we should exit if the next tile is too far out
			//(loop condition instead?)
			if (checkedTile.x >= newBottomRight.x)
			{
				//if any adjacent tiles are solid this would be false
				if (mOnOnewayPlatformIs)
					return true;
				break;
			}
		}
	}
	return false;
}

bool DPhoenix::Moving2DObject::HasCeiling(XMFLOAT2 oldPosition, XMFLOAT2 position,
	XMFLOAT2 speed, float* ceilingY, DPhoenix::Map* map)
{
	//old positions calculated too in case we are jumping too fast (bullet through paper)
	XMFLOAT2 oldCenter(oldPosition.x + mAABBOffset.x, oldPosition.y + mAABBOffset.y);
	XMFLOAT2 center(position.x + mAABBOffset.x, position.y + mAABBOffset.y);

	//calculate 1px high AABB at top of object's AABB - old position and new
	XMFLOAT2 oldTopRight(oldCenter.x + mAABB.halfSize.x - 1.0f, oldCenter.y + mAABB.halfSize.y - 1.0f);
	XMFLOAT2 newTopRight(center.x + mAABB.halfSize.x - 1.0f, center.y + mAABB.halfSize.y - 1.0f);
	XMFLOAT2 newTopLeft(newTopRight.x - mAABB.halfSize.x * 2.0f + 2.0f, newTopRight.y);

	//We start the search from the tile at the previous frame's sensor position, 
	//and end it at the current frame's sensor position. That's of course because 
	//when we check for a ceiling collision we assume we are jumping up, and that 
	//means we're moving from the lower position to the higher one.
	int endY = map->GetMapTileYAtPoint(oldTopRight.y);
	int begY = max(map->GetMapTileYAtPoint(oldTopRight.y) + 1, endY);
	//calculate distance between - max value should be 1
	int dist = max(abs(endY - begY), 1);
	//var declaration to use in checking
	int tileIndexX;

	//we start at the old position then check tiles up to the new position
	//(looping opposite way to ground checks)
	for (int tileIndexY = begY; tileIndexY <= endY; ++tileIndexY)
	{
		//interpolate between positions based on distance to get a bottom left
		//value to check against
		XMVECTOR topRightVec = XMVectorLerp(XMLoadFloat2(&newTopRight),
			XMLoadFloat2(&oldTopRight),
			(float)abs(endY - tileIndexY) / dist);
		//number conversions using XNA maths
		XMFLOAT2 topRight;
		XMStoreFloat2(&topRight, topRightVec);
		//and now for the right corner
		XMFLOAT2 topLeft(topRight.x - mAABB.halfSize.x * 2.0f + 2.0f, topRight.y);

		//here we check the ground collision along the width of the bottom sensor AABB
		for (XMFLOAT2 checkedTile = topLeft; ; checkedTile.x += map->mTileSize)
		{
			//make sure we're not moving too far out to the right
			checkedTile.x = min(checkedTile.x, topRight.x);

			//get the tile index
			tileIndexX = map->GetMapTileXAtPoint(checkedTile.x);

			//check if obstacle
			if (map->IsObstacle(tileIndexX, tileIndexY))
			{
				*ceilingY = (float)tileIndexY * map->mTileSize - 
					map->mTileSize / 2.0f + map->mPosition.y;
				return true;
			}

			//we should exit if the next tile is too far out
			if (checkedTile.x >= topRight.x)
			{
				break;
			}
		}
	}
	return false;
}

bool DPhoenix::Moving2DObject::CollidesWithLeftWall(XMFLOAT2 oldPosition, XMFLOAT2 position, 
		float * wallX, DPhoenix::Map * map)
{
	//old positions calculated too in case we are moving too fast
	XMFLOAT2 oldCenter(oldPosition.x + mAABBOffset.x, oldPosition.y + mAABBOffset.y);
	XMFLOAT2 center(position.x + mAABBOffset.x, position.y + mAABBOffset.y);

	//1 pixel AABB on left edge of object's AABB
	XMFLOAT2 oldBottomLeft(oldCenter.x - mAABB.halfSize.x + 1.0f, oldCenter.y - mAABB.halfSize.y + 1.0f);
	XMFLOAT2 newBottomLeft(center.x - mAABB.halfSize.x + 1.0f, center.y - mAABB.halfSize.y + 1.0f);
	XMFLOAT2 newTopLeft(newBottomLeft.x, center.y + mAABB.halfSize.y - 1.0f);

	//var declaration to use in checking
	int tileIndexY;
	//We start the search from the tile at the previous frame's sensor position, 
	//and end it at the current frame's sensor position. 
	//when we check for a left collision we assume we are moving left, and that 
	//means we're moving from the right position to the left one.
	int endX = map->GetMapTileXAtPoint(newBottomLeft.x);
	int begX = max(map->GetMapTileXAtPoint(oldBottomLeft.x) - 1, endX);
	int dist = max(abs(endX - begX), 1);

	//if at edge of map, bound as solid collision at left edge
	if (begX <= 0.0f)
	{
		*wallX = 0.0f;
		return true;
	}

	//LOOPING ON X FOR WALLS - backwards (to the left)
	for (int tileIndexX = begX; tileIndexX >= endX; --tileIndexX)
	{
		//interpolate between positions based on distance to get a bottom left
		//value to check against
		XMVECTOR bottomLeftVec = XMVectorLerp(XMLoadFloat2(&newBottomLeft),
			XMLoadFloat2(&oldBottomLeft),
			(float)abs(endX - tileIndexX) / dist);
		//number conversions using XNA maths
		XMFLOAT2 bottomLeft;
		XMStoreFloat2(&bottomLeft, bottomLeftVec);
		//and now for the top left
		XMFLOAT2 topLeft(bottomLeft.x, bottomLeft.y + mAABB.halfSize.y - 1.0f);

		//now loop on Y posiions to check collisions
		for (XMFLOAT2 checkedTile = bottomLeft; ; checkedTile.y += map->mTileSize)
		{
			//make sure we're not moving too far out to the right
			checkedTile.y = min(checkedTile.y, topLeft.y);

			//get the tile index
			tileIndexY = map->GetMapTileYAtPoint(checkedTile.y);

			//check if obstacle
			if (map->IsObstacle(tileIndexX, tileIndexY))
			{
				*wallX = (float)tileIndexX * map->mTileSize + map->mTileSize / 2.0f + map->mPosition.x;
				return true;
			}

			//we should exit if the next tile is too far out
			if (checkedTile.y >= topLeft.y)
			{
				break;
			}
		}
	}
	return false;
}

bool DPhoenix::Moving2DObject::CollidesWithRightWall(XMFLOAT2 oldPosition, XMFLOAT2 position,
	float * wallX, DPhoenix::Map * map)
{
	//old positions calculated too in case we are moving too fast
	XMFLOAT2 oldCenter(oldPosition.x + mAABBOffset.x, oldPosition.y + mAABBOffset.y);
	XMFLOAT2 center(position.x + mAABBOffset.x, position.y + mAABBOffset.y);

	//1 pixel AABB on right edge of object's AABB
	XMFLOAT2 oldBottomRight(oldCenter.x + mAABB.halfSize.x - 1.0f, oldCenter.y - mAABB.halfSize.y + 1.0f);
	XMFLOAT2 newBottomRight(center.x + mAABB.halfSize.x - 1.0f, center.y - mAABB.halfSize.y + 1.0f);
	XMFLOAT2 newTopRight(newBottomRight.x, center.y + mAABB.halfSize.y - 1.0f);
	//var declaration to use in checking
	int tileIndexY;

	//We start the search from the tile at the previous frame's sensor position, 
	//and end it at the current frame's sensor position. 
	//when we check for a right collision we assume we are moving right, and that 
	//means we're moving from the left position to the right one.
	int endX = map->GetMapTileXAtPoint(newBottomRight.x);
	int begX = max(map->GetMapTileXAtPoint(oldBottomRight.x) + 1, endX);
	int dist = max(abs(endX - begX), 1);
	
	//LOOPING ON X FOR WALLS - forwards (to the right)
	for (int tileIndexX = begX; tileIndexX <= endX; ++tileIndexX)
	{
		//interpolate between positions based on distance to get a bottom left
		//value to check against
		XMVECTOR bottomRightVec = XMVectorLerp(XMLoadFloat2(&newBottomRight),
			XMLoadFloat2(&oldBottomRight),
			(float)abs(endX - tileIndexX) / dist);
		//number conversions using XNA maths
		XMFLOAT2 bottomRight;
		XMStoreFloat2(&bottomRight, bottomRightVec);
		//and now for the top left
		XMFLOAT2 topRight(bottomRight.x, bottomRight.y + mAABB.halfSize.y - 1.0f);

		for (XMFLOAT2 checkedTile = bottomRight; ; checkedTile.y += map->mTileSize)
		{
			//make sure we're not moving too far out to the right
			checkedTile.y = min(checkedTile.y, topRight.y);

			//get the tile index
			tileIndexY = map->GetMapTileYAtPoint(checkedTile.y);

			//check if obstacle
			if (map->IsObstacle(tileIndexX, tileIndexY))
			{
				*wallX = (float)tileIndexX * map->mTileSize - map->mTileSize / 2.0f + map->mPosition.x;
				return true;
			}

			//we should exit if the next tile is too far out
			if (checkedTile.y >= topRight.y)
			{
				break;
			}
		}
	}
	return false;
}

