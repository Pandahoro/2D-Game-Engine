#pragma once
#include "Enemy.h"

namespace PandaEngine
{
	enum BossMoveStates
	{
		BOSS_STAND_MOVESTATE,
		BOSS_JUMP_MOVESTATE
	};

	enum BossActionStates
	{
		BOSS_FIRING_ACTIONSTATE,
		BOSS_NONFIRING_ACTIONSTATE
	};

	class Boss : public Enemy
	{
	public:
		//state vars
		BossMoveStates mMoveState;
		BossActionStates mActionState;
		//timers
		GameTimer mMoveTimer;
		//spritesheets
		Sprite* mBossStandSprite;
		Sprite* mBossJumpSprite;
		Sprite* mBossStandFireSprite;
		Sprite* mBossJumpFireSprite;
		//flags for jumping
		bool mJumpDownFlag;
		bool mJumpUpFlag;
		//vars for handling jumping physics
		float mJumpSpeed;
		float mRunSpeed;
		float mGravity;
		float mMaxFallSpeed;

		//constructor, takes position and device pointers
		Boss(TextureMgr* mTexMgr, ID3D11Device* md3dDevice, 
			XMFLOAT2 position, AudioMgr* audioMgr);
		~Boss() { };

		//update logic
		void EnemyUpdate(float deltaTime, PandaEngine::Map* map, 
			XMFLOAT2 playerPos);
		//handle damage
		bool TakeDamage(int dmg, AABB2D fireBox);
		//reset timers
		void ResetTimers();

	};
}

