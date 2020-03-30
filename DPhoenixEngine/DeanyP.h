#pragma once

#include "Moving2DObject.h"

namespace PandaEngine
{
	enum DeanyPInputs
	{
		DP_LEFT_INPUT,
		DP_RIGHT_INPUT,
		DP_UP_INPUT,
		DP_DOWN_INPUT,
		DP_JUMP_INPUT,
		DP_FIRE_INPUT,
		DP_REFLECT_INPUT,
		DP_MAX_INPUT	//used for indexing arrays - not 'safe'
						//if we do not use default numbering
	};

	enum DeanyPMoveStates
	{
		DP_STAND_MOVESTATE,
		DP_RUN_MOVESTATE,
		DP_JUMP_MOVESTATE
	};

	enum DeanyPActionStates
	{
		DP_FIRING_ACTIONSTATE,
		DP_NOTFIRING_ACTIONSTATE,
		DP_REFLECTING_ACTIONSTATE
	};

	enum DeanyPLifeStates
	{
		DP_OK_LIFESTATE,
		DP_HURT_LIFESTATE,
		DP_INV_LIFESTATE,
		DP_DIE_LIFESTATE,
		DP_GAMEOVER_LIFESTATE
	};

	enum DeanyPFireDirections
	{
		DP_FIRE_RIGHT,
		DP_FIRE_LEFT,
		DP_FIRE_MAX
	};

	class DeanyP : public Moving2DObject
	{
	public:
		//spritesheets
		Sprite* mDieSprite;			//dying
		Sprite* mIdleSprite;		//idle
		Sprite* mReflectSprite;		//reflecting
		Sprite* mRunSprite;			//running
		Sprite* mRunFFwdSprite;		//running and firing
		Sprite* mStandFFwdSprite;	//sanding and firing
		Sprite* mJumpSprite;		//jumping
		Sprite* mJumpFFwdSprite;	//jumping and firing

		//easy to use another pointer for current sprite
		Sprite* mCurrentSprite;

		float mJumpSpeed;		//jump speed
		float mRunSpeed;		//running speed
		float mGravity;			//gravity
		float mMaxFallSpeed;	//max descend / fall speed
		float mMinJumpSpeed;	//minimum jump speed (on tap)

		//arrays for inputs (previous and current)
		bool mInputs[DP_MAX_INPUT];
		bool mPrevInputs[DP_MAX_INPUT];

		//state machines for movment, actions and life status
		DeanyPMoveStates mMoveState;
		DeanyPActionStates mActionState;
		DeanyPLifeStates mLifeState;

		//flag if facing left (to flip sprite and animation)
		bool isFacingLeft;

		int mHealth;			//health
		int mMaxHealth;			//max health
		int mKnowledge;			//knowledge (fire gauge)
		int mMaxKnowledge;		//max knowledge
		bool mLastLifeFlag;		//are we on the last life?
		float mColorLerpValue;	//'lerp' amount between for life states

		int mLives;				//lives

		//timers for key actions
		GameTimer mFireTimer;		//for firing delay between 'bullets'
		GameTimer mReflectTimer;	//for delay in adding to knowledge gauge
		GameTimer mLifeTimer;		//for transition between life states

		bool mToFireFlag;								//ready to fire?
		DeanyPFireDirections mCurrentFireDirection;		//which direction to fire?
		XMFLOAT2 mFireDirections[DP_FIRE_MAX];			//array of fire directions

		//constructor / destructor
		DeanyP(TextureMgr* mTexMgr, ID3D11Device* md3dDevice,
			AudioMgr* audioMgr);
		~DeanyP() { };

		//update character logic
		void CharacterUpdate(float deltaTime, PandaEngine::Map* map);

		//key handling
		bool Released(DeanyPInputs key);
		bool KeyState(DeanyPInputs key);
		bool Pressed(DeanyPInputs key);

		void UpdatePrevInputs();

		//damage handling
		void TakeDamage(int dmg);
	};
}

