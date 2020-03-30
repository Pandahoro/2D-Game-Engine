#pragma once
#include "D3DUtil.h"
#include "AABB2D.h"

namespace DPhoenix
{
	//enumeration for types of fireball / projectile
	enum FireTypes
	{
		FIRE_PLAYER,
		FIRE_CHEMTRAILS,
		FIRE_SYRINGE
	};

	class Fire
	{
	public:
		Sprite* mFireSprite;	//spritesheet

		AudioMgr* mAudioMgr;	//pointer to audio manager

		XMFLOAT2 mPosition;		//position
		XMFLOAT2 mDirection;	//direction of movement
		float mSpeed;			//speed of movement

		//type flags for fire / projectile
		FireTypes mFireType;
		bool mIsFriendlyFire;

		AABB2D mAABB;			//bounding box

		//values for SUVAT projectiles
		bool isSuvatFlag;
		float mGravity;
		float mAngle;
		XMFLOAT2 mSuvatInitialVelocity;
		float mTimeElapsed;
		XMFLOAT2 mInitialPos;

		//constructor
		Fire(TextureMgr* mTexMgr, ID3D11Device* md3dDevice, XMFLOAT2 position, 
			XMFLOAT2 direction, FireTypes fireType, AudioMgr* audioMgr);
		~Fire() { };
		//update logic
		void Update(float deltaTime);
	};
}

