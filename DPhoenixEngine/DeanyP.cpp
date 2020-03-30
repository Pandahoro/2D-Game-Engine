#include "DeanyP.h"

DPhoenix::DeanyP::DeanyP(TextureMgr* mTexMgr, ID3D11Device* md3dDevice, AudioMgr* audioMgr)
{
	//initilaise ALL the SPRITES -----------------------------------------------
	//(big section)

#pragma region SpritesheetInit
	//dying
	mDieSprite = new DPhoenix::Sprite();
	mDieSprite->Load("Textures\\DeanyP\\Die.png", mTexMgr, 60.0f, 100.0f, md3dDevice);
	mDieSprite->SetCurrentFrame(0);	mDieSprite->mAnimationColumns = 16;
	mDieSprite->mAnimationDirection = 12;	mDieSprite->SetAnimationRange(0, 15);
	mDieSprite->SetScale(1.0f);	mDieSprite->mFlipValue = 1.0f;
	//idle
	mIdleSprite = new DPhoenix::Sprite();
	mIdleSprite->Load("Textures\\DeanyP\\Idle.png", mTexMgr, 50.0f, 100.0f, md3dDevice);
	mIdleSprite->SetCurrentFrame(0);	mIdleSprite->mAnimationColumns = 16;
	mIdleSprite->mAnimationDirection = 10;	mIdleSprite->SetAnimationRange(0, 15);
	mIdleSprite->SetScale(1.0f);	mIdleSprite->mFlipValue = 1.0f;
	//reflecting (charging)
	mReflectSprite = new DPhoenix::Sprite();
	mReflectSprite->Load("Textures\\DeanyP\\Reflect.png", mTexMgr, 50.0f, 100.0f, md3dDevice);
	mReflectSprite->SetCurrentFrame(0);	mReflectSprite->mAnimationColumns = 8;
	mReflectSprite->mAnimationDirection = 7;	mReflectSprite->SetAnimationRange(0, 7);
	mReflectSprite->SetScale(1.0f);	mReflectSprite->mFlipValue = 1.0f;
	//running
	mRunSprite = new DPhoenix::Sprite();
	mRunSprite->Load("Textures\\DeanyP\\Run.png", mTexMgr, 75.0f, 100.0f, md3dDevice);
	mRunSprite->SetCurrentFrame(0);	mRunSprite->mAnimationColumns = 12;
	mRunSprite->mAnimationDirection = 16;	mRunSprite->SetAnimationRange(0, 11);
	mRunSprite->SetScale(1.0f);	mRunSprite->mFlipValue = 1.0f;
	//running and gunning
	mRunFFwdSprite = new DPhoenix::Sprite();
	mRunFFwdSprite->Load("Textures\\DeanyP\\RunFFwd.png", mTexMgr, 75.0f, 100.0f, md3dDevice);
	mRunFFwdSprite->SetCurrentFrame(0);	mRunFFwdSprite->mAnimationColumns = 12;
	mRunFFwdSprite->mAnimationDirection = 16;	mRunFFwdSprite->SetAnimationRange(0, 11);
	mRunFFwdSprite->SetScale(1.0f);	mRunFFwdSprite->mFlipValue = 1.0f;
	//standing and firing
	mStandFFwdSprite = new DPhoenix::Sprite();
	mStandFFwdSprite->Load("Textures\\DeanyP\\StandFFwd.png", mTexMgr, 55.0f, 100.0f, md3dDevice);
	mStandFFwdSprite->SetCurrentFrame(0);	mStandFFwdSprite->mAnimationColumns = 4;
	mStandFFwdSprite->mAnimationDirection = 16;	mStandFFwdSprite->SetAnimationRange(0, 3);
	mStandFFwdSprite->SetScale(1.0f);	mStandFFwdSprite->mFlipValue = 1.0f;
	//jumping
	mJumpSprite = new DPhoenix::Sprite();
	mJumpSprite->Load("Textures\\DeanyP\\Jump.png", mTexMgr, 55.0f, 100.0f, md3dDevice);
	mJumpSprite->SetCurrentFrame(0);	mJumpSprite->mAnimationColumns = 4;
	mJumpSprite->mAnimationDirection = 16;	mJumpSprite->SetAnimationRange(0, 3);
	mJumpSprite->SetScale(1.0f);	mJumpSprite->mFlipValue = 1.0f;
	//jumping and firing
	mJumpFFwdSprite = new DPhoenix::Sprite();
	mJumpFFwdSprite->Load("Textures\\DeanyP\\JumpFFwd.png", mTexMgr, 55.0f, 100.0f, md3dDevice);
	mJumpFFwdSprite->SetCurrentFrame(0);	mJumpFFwdSprite->mAnimationColumns = 4;
	mJumpFFwdSprite->mAnimationDirection = 16;	mJumpFFwdSprite->SetAnimationRange(0, 3);
	mJumpFFwdSprite->SetScale(1.0f);	mJumpFFwdSprite->mFlipValue = 1.0f;

	//set the current sprite as the idle animation
	mCurrentSprite = mIdleSprite;

	//END Sprite Initialisation ------- WHOAH... ------------------------------------------------------
#pragma endregion SpritesheetInit

	//these values to be tweaked for the right feel 
	//for your mechanics
	mJumpSpeed = 700.0f;
	mRunSpeed = 450.0f;

	mGravity = -1500.0f;
	mMaxFallSpeed = -1500.0f;
	mMinJumpSpeed = 400.0f;

	//initialise input arrays to be false 
	for (int i = 0; i < DP_MAX_INPUT; i++)
	{
		mInputs[i] = false;
		mPrevInputs[i] = false;
	}
	//set initial states
	mMoveState = DP_STAND_MOVESTATE;
	mActionState = DP_NOTFIRING_ACTIONSTATE;
	mLifeState = DP_OK_LIFESTATE;

	//flags and stats init
	isFacingLeft = false;
	mHealth = 100; mMaxHealth = 100;
	mKnowledge = 100; mMaxKnowledge = 100;
	mLastLifeFlag = false;
	mColorLerpValue = 0.0f;

	//parent class initialisations
	mPosition.x = 0.0f; mPosition.y = 0.0f;
	mAABB.center.x = 0.0f; mAABB.center.y = 0.0f;
	mAABB.halfSize.x = 15.0f; mAABB.halfSize.y = 50.0f;
	mAABBOffset.x = 0.0f; mAABBOffset.y = 0.0f;

	//collision flags
	mPushRightWallJust = false; mPushRightWallIs = false;
	mPushLeftWallJust = false; mPushLeftWallIs = false;

	mOnGroundJust = true; mOnGroundIs = true;
	mAtCeilingJust = false; mAtCeilingIs = false;

	//timers initialisation
	mFireTimer.Start();
	mReflectTimer.Start();
	mLifeTimer.Start();

	mOneWayPlatformThreshold = 2.0f;
	mOnOnewayPlatformIs = false;

	//fire init
	mCurrentFireDirection = DP_FIRE_RIGHT;
	mFireDirections[DP_FIRE_RIGHT] = XMFLOAT2(1.0f, 0.0f);
	mFireDirections[DP_FIRE_LEFT] = XMFLOAT2(-1.0f, 0.0f);
	mToFireFlag = false;

	mLives = 3;

	//set for collision checks
	mIsTileCollider = true;
	mIsWallCollider = true;

}

void DPhoenix::DeanyP::CharacterUpdate(float deltaTime, DPhoenix::Map* map)
{
	//reset firing flag
	mToFireFlag = false;

	switch (mLifeState)
	{
	case DP_OK_LIFESTATE:
	case DP_HURT_LIFESTATE:
	case DP_INV_LIFESTATE:
	{
		switch (mMoveState)
		{
		case DP_STAND_MOVESTATE:
		{
			//no movement
			mSpeed.x = 0.0f; mSpeed.y = 0.0f;

			//if not on ground, set to jump
			if (!mOnGroundIs)
			{
				mMoveState = DP_JUMP_MOVESTATE;
				break;
			}

			//Change sprites if firing --------------------------------------------

			if (KeyState(DP_FIRE_INPUT))
			{
				mActionState = DP_FIRING_ACTIONSTATE;

				if (KeyState(DP_RIGHT_INPUT))
				{
					mCurrentSprite = mStandFFwdSprite;
					isFacingLeft = false;
				}
				else if (KeyState(DP_LEFT_INPUT))
				{
					mCurrentSprite = mStandFFwdSprite;
					isFacingLeft = true;
				}
			}
			else
			{
				if (KeyState(DP_REFLECT_INPUT))
				{
					mActionState = DP_REFLECTING_ACTIONSTATE;
					//stand / idle sprite?
					mCurrentSprite = mReflectSprite;
				}
				else
				{
					mActionState = DP_NOTFIRING_ACTIONSTATE;
					//stand / idle sprite?
					mCurrentSprite = mIdleSprite;
				}

				//if we are going left or right (so they wouldn't match)
				if (KeyState(DP_RIGHT_INPUT) != KeyState(DP_LEFT_INPUT))
				{
					mMoveState = DP_RUN_MOVESTATE;
				}
				//or are we jumping now? (should this be else if?)
				else if (KeyState(DP_JUMP_INPUT) && KeyState(DP_DOWN_INPUT))
				{
					//move us below the threshold if one way
					if (mOnOnewayPlatformIs)
						mPosition.y -= mOneWayPlatformThreshold;
				}
				//or are we jumping now? (should this be else if?)
				else if (KeyState(DP_JUMP_INPUT))
				{
					mSpeed.y = mJumpSpeed;
					mMoveState = DP_JUMP_MOVESTATE;
				}
				break;

			}
			//end firing ----------------------------------------------------------
		}
		break;
		case DP_RUN_MOVESTATE:
		{
			//run animation?
			//mCurrentSprite = mRunSprite;
			//should only go one direction:
			if (KeyState(DP_RIGHT_INPUT) == KeyState(DP_LEFT_INPUT))
			{
				mMoveState = DP_STAND_MOVESTATE;
				mSpeed.x = 0.0f; mSpeed.y = 0.0f;
				break;
			}
			else if (KeyState(DP_RIGHT_INPUT))
			{
				//check if against a wall
				if (mPushRightWallIs)
					mSpeed.x = 0.0f;
				else
					mSpeed.x = mRunSpeed;

				//firing check on the right --------------

				if (KeyState(DP_FIRE_INPUT))
				{
					mActionState = DP_FIRING_ACTIONSTATE;
					mCurrentSprite = mRunFFwdSprite;
				}
				else
				{
					mActionState = DP_NOTFIRING_ACTIONSTATE;
					//run sprite
					mCurrentSprite = mRunSprite;
				}
				//end firing check on the right ----------------------

				isFacingLeft = false;
				mCurrentSprite->mAnimationDirection = 16;
			}
			else if (KeyState(DP_LEFT_INPUT))
			{
				//check if against a wall
				if (mPushLeftWallIs)
					mSpeed.x = 0.0f;
				else
					mSpeed.x = -mRunSpeed;

				//firing check on the left --------------

				if (KeyState(DP_FIRE_INPUT))
				{
					mActionState = DP_FIRING_ACTIONSTATE;
					mCurrentSprite = mRunFFwdSprite;
				}
				else
				{
					mActionState = DP_NOTFIRING_ACTIONSTATE;
					//stand / idle sprite?
					mCurrentSprite = mRunSprite;
				}
				//end firing check on the left ----------------------

				isFacingLeft = true;
				mCurrentSprite->mAnimationDirection = -16;
			}
			//now for a jump check
			if (KeyState(DP_JUMP_INPUT))
			{
				mSpeed.y = mJumpSpeed;
				//play jump sound here?
				mMoveState = DP_JUMP_MOVESTATE;
				break;
			}
			else if (!mOnGroundIs)
			{
				mMoveState = DP_JUMP_MOVESTATE;
			}
		}
		break;
		case DP_JUMP_MOVESTATE:
		{
			//jump animation?
			mCurrentSprite = mJumpSprite;
			//gravity / falling values
			mSpeed.y += mGravity * deltaTime;
			mSpeed.y = max(mSpeed.y, mMaxFallSpeed);

			//now make jump higher if button pressed longer
			//(needs experimentation)
			if (!KeyState(DP_JUMP_INPUT) && mSpeed.y > 0.0f)
				mSpeed.y = min(mSpeed.y, mMinJumpSpeed);

			//movement here same as in run state
			if (KeyState(DP_RIGHT_INPUT) == KeyState(DP_LEFT_INPUT))
			{
				mSpeed.x = 0.0f;

				//firing checks ------------------------

				if (KeyState(DP_FIRE_INPUT))
				{
					mActionState = DP_FIRING_ACTIONSTATE;

					mCurrentSprite = mJumpFFwdSprite;
				}
				else
				{
					mActionState = DP_NOTFIRING_ACTIONSTATE;
					//jump sprite?
					mCurrentSprite = mJumpSprite;
				}
				//end firing checks --------------------

			}
			else if (KeyState(DP_RIGHT_INPUT))
			{
				//check if against a wall
				if (mPushRightWallIs)
					mSpeed.x = 0.0f;
				else
					mSpeed.x = mRunSpeed;

				isFacingLeft = false;

				//firing checks ------------------------

				if (KeyState(DP_FIRE_INPUT))
				{
					mActionState = DP_FIRING_ACTIONSTATE;
					mCurrentSprite = mJumpFFwdSprite;
				}
				else
				{
					mActionState = DP_NOTFIRING_ACTIONSTATE;
					mCurrentSprite = mJumpSprite;
				}
				//end firing checks --------------------
			}
			else if (KeyState(DP_LEFT_INPUT))
			{
				//check if against a wall
				if (mPushLeftWallIs)
					mSpeed.x = 0.0f;
				else
					mSpeed.x = -mRunSpeed;

				isFacingLeft = true;

				//firing checks ------------------------
				if (KeyState(DP_FIRE_INPUT))
				{
					mActionState = DP_FIRING_ACTIONSTATE;
					mCurrentSprite = mJumpFFwdSprite;
				}
				else
				{
					mActionState = DP_NOTFIRING_ACTIONSTATE;
					mCurrentSprite = mJumpSprite;
				}
				//end firing checks --------------------
			}

			//if we hit the ground
			if (mOnGroundIs)
			{
				//if there's no movement change state to standing
				if (mInputs[(int)DP_RIGHT_INPUT] == KeyState(DP_LEFT_INPUT))
				{
					mMoveState = DP_STAND_MOVESTATE;
					mSpeed.x = 0.0f; mSpeed.y = 0.0f;
				}
				else  //either go right or go left are pressed so we change the state to run
				{
					mMoveState = DP_RUN_MOVESTATE;
					mSpeed.y = 0.0f;
				}
			}
		}
		break;
		}
		//end switch /case move state check

		switch (mActionState)
		{
		case DP_FIRING_ACTIONSTATE:
			//tick the fire timer
			mFireTimer.Tick();
			//if 0.1 second passed
			if (mFireTimer.TotalTime() >= 0.1f)
			{
				//check we have knowledge to fire
				if (mKnowledge > 0)
				{
					//set fire flag
					mToFireFlag = true;
					//deduct knowledge and bound to 0
					mKnowledge -= 5;
					if (mKnowledge < 0)
						mKnowledge = 0;
					//reset timer
					mFireTimer.Reset();
					//set fire direction (basic with two way firing)
					//would set above in full logic checks for 8-way etc.
					if (isFacingLeft)
						mCurrentFireDirection = DP_FIRE_LEFT;
					else
						mCurrentFireDirection = DP_FIRE_RIGHT;
				}
			}
			break;
		case DP_REFLECTING_ACTIONSTATE:
			//tick the reflect timer
			mReflectTimer.Tick();
			//if 0.1 second passed
			if (mReflectTimer.TotalTime() >= 0.1f)
			{
				//update and bound knowledge by max
				mKnowledge += 5;
				if (mKnowledge > mMaxKnowledge)
					mKnowledge = mMaxKnowledge;
				//reset timer
				mReflectTimer.Reset();
			}
			break;
		}
		//end switch / case action state check

		//if lifestate is ok, no color lerp
		if (mLifeState == DP_OK_LIFESTATE)
		{
			mColorLerpValue = 0.0f;
			mCurrentSprite->mColorLerpValue = mColorLerpValue;
			mCurrentSprite->mAltColorValue = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
		//if we are hurt...
		if (mLifeState == DP_HURT_LIFESTATE)
		{
			mLifeTimer.Tick();						//tick timer
			mColorLerpValue -= 1.0f * deltaTime;	//deduct value from lerp value (color fades)
			mCurrentSprite->mColorLerpValue = mColorLerpValue;
			mCurrentSprite->mAltColorValue = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);	//alt color is red

			//after half a second
			if (mLifeTimer.TotalTime() >= 0.5f)
			{
				//reset timer, color, lerp value and life state
				mColorLerpValue = 0.0f;
				mCurrentSprite->mColorLerpValue = mColorLerpValue;
				mLifeState = DP_OK_LIFESTATE;
				mLifeTimer.Reset();
			}
		}
		//if we are invincible (following respawn)
		if (mLifeState == DP_INV_LIFESTATE)
		{
			mLifeTimer.Tick();						//tick timer
			mColorLerpValue -= 0.5f * deltaTime;	//deduct value from lerp value (color fades)
			mCurrentSprite->mColorLerpValue = mColorLerpValue;
			mCurrentSprite->mAltColorValue = XMFLOAT4(0.7f, 0.7f, 0.0f, 1.0f);	//alt color is 'gold'
			//after a second
			if (mLifeTimer.TotalTime() >= 1.0f)
			{
				//reset timer, color, lerp value and life state
				mColorLerpValue = 0.0f;
				mCurrentSprite->mColorLerpValue = mColorLerpValue;
				mLifeState = DP_OK_LIFESTATE;
				mLifeTimer.Reset();
			}
		}

	}
	break;
	case DP_DIE_LIFESTATE:
	{
		//no movement when dying - tick timer
		mSpeed.x = 0.0f; mSpeed.y = 0.0f;
		mLifeTimer.Tick();
		//after 1.3 seconds (how long the animation lasts)
		if (mLifeTimer.TotalTime() >= 1.3f)
		{	//if not the last life...
			if (!mLastLifeFlag)
			{
				//update lerp values, life state, health and reset timer
				mColorLerpValue = 0.5f;
				mCurrentSprite->mColorLerpValue = mColorLerpValue;
				mLifeState = DP_INV_LIFESTATE;
				mLifeTimer.Reset();
				mHealth = 100;
			}
			else
			{
				//otherwise set state to flag for game over
				mLifeState = DP_GAMEOVER_LIFESTATE;
			}
		}
	}
	break;
	}

	UpdatePhysics(deltaTime, map);

	UpdatePrevInputs();

	//current sprite updates
	mCurrentSprite->Update(deltaTime);
	if (isFacingLeft)
		mCurrentSprite->mFlipValue = -1.0f;
	else
		mCurrentSprite->mFlipValue = 1.0f;

	mCurrentSprite->mPosition.x = mPosition.x;
	mCurrentSprite->mPosition.y = mPosition.y;
}

//if the 'key' isn't currently pressed but was on last frame
bool DPhoenix::DeanyP::Released(DeanyPInputs key)
{
	return (!mInputs[(int)key] && mPrevInputs[(int)key]);
}

//if the 'key' is presssed
bool DPhoenix::DeanyP::KeyState(DeanyPInputs key)
{
	return (mInputs[(int)key]);
}

//if the 'key' is pressed and wasn't on last frame 
bool DPhoenix::DeanyP::Pressed(DeanyPInputs key)
{
	return (mInputs[(int)key] && !mPrevInputs[(int)key]);
}

//transfer current input values to previous inputs
void DPhoenix::DeanyP::UpdatePrevInputs()
{
	for (int i = 0; i < DP_MAX_INPUT; i++)
	{
		mPrevInputs[i] = mInputs[i];
		//flush for new inputs on next frame
		mInputs[i] = false;
	}

}

//handle damage ---
void DPhoenix::DeanyP::TakeDamage(int dmg)
{
	//if OK
	if (mLifeState == DP_OK_LIFESTATE)
	{
		//subtracts damage from health
		mHealth -= dmg;
		//if health run out
		if (mHealth <= 0)
		{
			//set to diying lifestate - change sprite properties
			mLifeState = DP_DIE_LIFESTATE;
			mCurrentSprite = mDieSprite;
			mCurrentSprite->SetCurrentFrame(0);
			mColorLerpValue = 0.0f;
			mCurrentSprite->mColorLerpValue = mColorLerpValue;
			//decrement lives
			mLives--;
			//if we have run out of lives
			if (mLives < 0)
			{
				//boun lives value and set flag for last life over
				mLives = 0;
				mLastLifeFlag = true;
			}
			//based on where we are facing set the animation direction
			if (isFacingLeft)
			{
				mCurrentSprite->mAnimationDirection = -12;
			}
			else
			{
				mCurrentSprite->mAnimationDirection = 12;
			}
		}
		else
		{
			//if not dead, set to hurt lifestate and adjust color lerp
			mLifeState = DP_HURT_LIFESTATE;
			mColorLerpValue = 0.5f;
			mCurrentSprite->mColorLerpValue = mColorLerpValue;
		}
		//reset life timer
		mLifeTimer.Reset();
	}
}

