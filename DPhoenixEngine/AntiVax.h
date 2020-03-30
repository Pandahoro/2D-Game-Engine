#pragma once
#include "Enemy.h"

namespace PandaEngine
{
	enum AntiVaxMoveStates
	{
		ANTIVAX_STAND_MOVESTATE,
		ANTIVAX_JUMP_MOVESTATE
	};

	enum AntiVaxActionStates
	{
		ANTIVAX_FIRING_ACTIONSTATE,
		ANTIVAX_NONFIRING_ACTIONSTATE
	};

	class AntiVax : public Enemy
	{
	public:
		//state vars
		AntiVaxMoveStates mMoveState;
		AntiVaxActionStates mActionState;
		//timers
		GameTimer mMoveTimer;
		//spritesheets
		Sprite* mAntiVaxStandSprite;
		Sprite* mAntiVaxJumpSprite;
		Sprite* mAntiVaxStandFireSprite;
		Sprite* mAntiVaxJumpFireSprite;
		//flags for jumping
		bool mJumpDownFlag;
		bool mJumpUpFlag;
		//vars for handling jumping physics
		float mJumpSpeed;
		float mRunSpeed;
		float mGravity;
		float mMaxFallSpeed;

		//constructor, takes position and device pointers
		AntiVax(TextureMgr* mTexMgr, ID3D11Device* md3dDevice, 
			XMFLOAT2 position, AudioMgr* audioMgr);
		~AntiVax() { };

		//update logic
		void EnemyUpdate(float deltaTime, PandaEngine::Map* map, 
			XMFLOAT2 playerPos);
		//handle damage
		bool TakeDamage(int dmg, AABB2D fireBox);
		//reset timers
		void ResetTimers();

	};
}

