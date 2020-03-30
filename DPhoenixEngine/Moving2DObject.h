#pragma once
#include "D3DUtil.h"
#include "Map.h"

namespace PandaEngine
{

	class Moving2DObject
	{
	public:
		//position values
		XMFLOAT2 mOldPosition;
		XMFLOAT2 mPosition;
		//speed values
		XMFLOAT2 mOldSpeed;
		XMFLOAT2 mSpeed;
		//scale values
		XMFLOAT2 mScale;
		//bounding box for collision and offset from sprite centre
		AABB2D mAABB;
		XMFLOAT2 mAABBOffset;

		//collision flags ('just' was last frame, 
		//'Is' is current frame)
		bool mPushRightWallJust;
		bool mPushRightWallIs;

		bool mPushLeftWallJust;
		bool mPushLeftWallIs;

		bool mOnGroundJust;
		bool mOnGroundIs;

		bool mAtCeilingJust;
		bool mAtCeilingIs;

		//for one way platforms
		//as with others, init in DeanyP constructor
		float mOneWayPlatformThreshold;
		bool mOnOnewayPlatformIs;

		//determines if we are checking tile collision
		bool mIsTileCollider;
		bool mIsWallCollider;

		Moving2DObject() { };
		~Moving2DObject() { };

		//update physics for object (add map pointer)
		void UpdatePhysics(float deltaTime, PandaEngine::Map* map);
		bool HasGround(XMFLOAT2 oldPosition, XMFLOAT2 position, XMFLOAT2 speed,
			float* groundY, PandaEngine::Map* map);
		bool HasCeiling(XMFLOAT2 oldPosition, XMFLOAT2 position, XMFLOAT2 speed,
			float* ceilingY, PandaEngine::Map* map);
		bool CollidesWithLeftWall(XMFLOAT2 oldPosition, XMFLOAT2 position,
			float* wallX, PandaEngine::Map* map);
		bool CollidesWithRightWall(XMFLOAT2 oldPosition, XMFLOAT2 position,
			float* wallX, PandaEngine::Map* map);

	};

}

