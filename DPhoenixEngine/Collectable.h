#pragma once
#include "D3DUtil.h"
#include "AABB2D.h" //for AABB Struct

namespace DPhoenix
{
	enum CollectableTypes
	{
		COLLECTABLE_RESEARCH,
		COLLECTABLE_COFFEE
	};

	class Collectable
	{
	public:
		//Bounding box vars
		AABB2D mAABB;
		XMFLOAT2 mAABBOffset;
		//spritesheet to display
		Sprite* mCollectibleSprite;
		//type of collectable
		CollectableTypes mCollectableType;
		//position
		XMFLOAT2 mPosition;
		//timer values (if temp collectab;l, e.g. drops)
		bool mIsOnTimer;
		GameTimer mLifeTimer;
		float mLifetime;
		//flag for removal from scene
		bool mRemoveFlag;
		//constructor
		Collectable(TextureMgr* mTexMgr, ID3D11Device* md3dDevice,
			XMFLOAT2 mPosition,
			CollectableTypes type, bool isOnTimer = false);
		~Collectable() { };
		//update
		void Update(float dt);
	};
}
