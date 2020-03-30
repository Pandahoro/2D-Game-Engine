#pragma once
#include "D3DUtil.h"
#include "AABB2D.h"

namespace PandaEngine
{
	class Angel
	{
	public:
		//spritesheet
		Sprite* mAngelSprite;
		//values for position and movement
		XMFLOAT2 mPosition;
		XMFLOAT2 mDirection;
		float mSpeed;
		float mAcceleration;
		//bounding box
		AABB2D mAABB;
		//get position and direction and device pointers to construct
		Angel(TextureMgr* mTexMgr, ID3D11Device* md3dDevice,
			XMFLOAT2 position, XMFLOAT2 direction);
		~Angel() { };
		//update logic
		void Update(float deltaTime);
	};
}

