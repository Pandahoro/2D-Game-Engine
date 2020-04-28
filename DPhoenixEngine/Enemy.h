#pragma once
#include "Moving2DObject.h"
#include "Fire.h"

namespace PandaEngine
{
	enum EnemyTypes
	{
		ENEMY_CHEMPLANE,
		ENEMY_ANTIVAX,
		ENEMY_BOSS
	};

	enum EnemyLifeStates
	{
		ENEMY_OK_LIFESTATE,
		ENEMY_HURT_LIFESTATE,
		ENEMY_INV_LIFESTATE,
		ENEMY_DIE_LIFESTATE
	};

	class Enemy : public Moving2DObject
	{
	public:
		//enemy type 
		EnemyTypes mEnemyType;

		//initial spawn point
		XMFLOAT2 mInitialSpawnPos;

		//for firing, where from, which direction and what type
		XMFLOAT2 mFireOffset;
		XMFLOAT2 mFireDirection;
		FireTypes mFireType;
		GameTimer mFireTimer;
		bool mToFireFlag;		//ready to fire

		//life state
		EnemyLifeStates mLifeState;

		//(you can have multiple bounding boxes for different actions)
		//(e.g. you may want to add areas where enemies absorb or reflect shots)
		AABB2D mWeakHitBox;			//weak area to take damage
		XMFLOAT2 mWeakHitBBOffset;

		//life timer and color lerp value
		GameTimer mLifeTimer;
		float mColorLerpValue;
		//pointer to current sprite sheet for display
		Sprite* mCurrentSprite;
		//audio manager pointer so enemies can playback sound independently
		AudioMgr* mAudioMgr;
				
		//flags
		bool mIsActive;		//is active (on screen, so updates occur)
		bool isFacingLeft;	//where the enemy is facing to determine flip value
		bool isHazard;		//if marked as hazard, colliding with the enemy takes dmg
		//health values
		int mHealth;		
		int mMaxHealth;
		//marked for removal
		bool mRemoveFlag;

		//empty constructors / destrictors - intended to be only parent class
		Enemy() { };
		~Enemy() {};

		//update method to be overwritten by child classes
		virtual void EnemyUpdate(float deltaTime, PandaEngine::Map* map, XMFLOAT2 playerPos) { };
		//method to take damage
		virtual bool TakeDamage(int dmg, AABB2D fireBox) { return false; };
		//method to reset timers - useful if offscreen
		virtual void ResetTimers() { };
	};
}

