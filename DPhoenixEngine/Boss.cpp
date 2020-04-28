#include "Boss.h"

PandaEngine::Boss::Boss(TextureMgr * mTexMgr, ID3D11Device * md3dDevice, 
	XMFLOAT2 position, AudioMgr* audioMgr)
{
	//audio manager pointer
	mAudioMgr = audioMgr;
	//spritsheet initialisation - standing animation
	mBossStandSprite = new PandaEngine::Sprite();
	mBossStandSprite->Load("Textures\\Enemies\\BossStand.png", mTexMgr, 90.0f, 100.0f, md3dDevice);
	mBossStandSprite->SetCurrentFrame(0);	mBossStandSprite->mAnimationColumns = 4;
	mBossStandSprite->mAnimationDirection = 10;	mBossStandSprite->SetAnimationRange(0, 3);
	mBossStandSprite->SetScale(3.0f);	mBossStandSprite->mFlipValue = 1.0f;
	//spritsheet initialisation - jump animation
	mBossJumpSprite = new PandaEngine::Sprite();
	mBossJumpSprite->Load("Textures\\Enemies\\BossStand.png", mTexMgr, 90.0f, 100.0f, md3dDevice);
	mBossJumpSprite->SetCurrentFrame(0);	mBossJumpSprite->mAnimationColumns = 4;
	mBossJumpSprite->mAnimationDirection = 10;	mBossJumpSprite->SetAnimationRange(0, 3);
	mBossJumpSprite->SetScale(3.0f);	mBossJumpSprite->mFlipValue = 1.0f;
	//spritsheet initialisation - standing and firing animation
	mBossStandFireSprite = new PandaEngine::Sprite();
	mBossStandFireSprite->Load("Textures\\Enemies\\Bossfire.png", mTexMgr, 90.0f, 100.0f, md3dDevice);
	mBossStandFireSprite->SetCurrentFrame(0);	mBossStandFireSprite->mAnimationColumns = 4;
	mBossStandFireSprite->mAnimationDirection = 16;	mBossStandFireSprite->SetAnimationRange(0, 3);
	mBossStandFireSprite->SetScale(3.0f);	mBossStandFireSprite->mFlipValue = 1.0f;
	//spritsheet initialisation - jumping and firing animation
	mBossJumpFireSprite = new PandaEngine::Sprite();
	mBossJumpFireSprite->Load("Textures\\Enemies\\Bossfire.png", mTexMgr, 90.0f, 100.0f, md3dDevice);
	mBossJumpFireSprite->SetCurrentFrame(0);	mBossJumpFireSprite->mAnimationColumns = 4;
	mBossJumpFireSprite->mAnimationDirection = 16;	mBossJumpFireSprite->SetAnimationRange(0, 3);
	mBossJumpFireSprite->SetScale(3.0f);	mBossJumpFireSprite->mFlipValue = 1.0f;
	//set current sprite nitially to standing sprite
	mCurrentSprite = mBossStandSprite;

	//initially facing left
	isFacingLeft = true;

	//flags
	mIsActive = false;		//not active till onscreen
	mToFireFlag = false;	//set only when firing
	mRemoveFlag = false;	//set when dead / marked for removal
	isHazard = false;		//enemy not a collison hazard

	//health values
	mHealth = 300; mMaxHealth = 300;

	//state initialisations
	mEnemyType = ENEMY_BOSS;
	mMoveState = BOSS_STAND_MOVESTATE;
	mActionState = BOSS_NONFIRING_ACTIONSTATE;
	mLifeState = ENEMY_OK_LIFESTATE;

	//parent class initialisations
	mInitialSpawnPos = position;
	mPosition = position;
	//tile collison box
	mAABB.center.x = 0.0f; mAABB.center.y = 0.0f;
	mAABB.halfSize.x = 25.0f; mAABB.halfSize.y = 50.0f;
	mAABBOffset.x = 0.0f; mAABBOffset.y = 0.0f;

	//main hit boxes
	mWeakHitBox.center.x = 0.0f; mWeakHitBox.center.y = 0.0f;
	mWeakHitBox.halfSize.x = 25.0f; mWeakHitBox.halfSize.y = 50.0f;
	mWeakHitBBOffset.x = 0.0f; mWeakHitBBOffset.y = 0.0f;

	//flags for collison
	mPushRightWallJust = false; mPushRightWallIs = false;
	mPushLeftWallJust = false; mPushLeftWallIs = false;
	mOnGroundJust = true; mOnGroundIs = true;
	mAtCeilingJust = false; mAtCeilingIs = false;

	//flags set so ground / ceiling checks in place
	mIsTileCollider = true;
	mIsWallCollider = false;

	mRunSpeed = 200.0f;
	mJumpSpeed = 700.0f;

	//might need to change this - no value given on tutorial
	mGravity = -1500.0f;
	mMaxFallSpeed = -1500.0f;

	//for one way platforms
	mOneWayPlatformThreshold = 2.0f;
	mOnOnewayPlatformIs = false;

	//begin timers
	mFireTimer.Start();
	mMoveTimer.Start();

	//firing vars initialisation
	mFireOffset.x = 0.0f; mFireOffset.y = 0.0f;
	mFireDirection.x = 1.0f; mFireDirection.y = 0.0f;
	mFireType = FIRE_SYRINGE;

	//flags for movements
	mJumpUpFlag = false;
	mJumpDownFlag = false;
}

//takes in player position to determine movement
void PandaEngine::Boss::EnemyUpdate(float deltaTime, PandaEngine::Map * map, 
	XMFLOAT2 playerPos)
{
	//only perform update logic is active flag set
	if (mIsActive)
	{
		//reset fire flag on each update
		mToFireFlag = false;

		//check life state
		switch (mLifeState)
		{
		case ENEMY_OK_LIFESTATE:
		case ENEMY_HURT_LIFESTATE:
		case ENEMY_INV_LIFESTATE:
		{
			//check movestate
			switch (mMoveState)
			{
			case BOSS_STAND_MOVESTATE:
			{
				//no movement
				mSpeed.x = 0.0f; mSpeed.y = 0.0f;

				//if not on ground, set to jump
				if (!mOnGroundIs)
				{
					mMoveState = BOSS_JUMP_MOVESTATE;
					break;
				}

				//determine direction to face based on player x position
				if (playerPos.x > mPosition.x + mAABB.halfSize.x)
				{
					isFacingLeft = false;
				}
				else if (playerPos.x < mPosition.x - mAABB.halfSize.x)
				{
					isFacingLeft = true;
				}
				else
				{
					isFacingLeft = false;
				}

				//Change sprites if firing --------------------------------------------
				if (mActionState == BOSS_FIRING_ACTIONSTATE)
				{
					mCurrentSprite = mBossStandFireSprite;
				}
				else
				{
					mCurrentSprite = mBossStandSprite;
				}
				//tick the move timer
				mMoveTimer.Tick();
				//should prob const this value / no magic numbers etc.
				//after 3 seconds
				if (mMoveTimer.TotalTime() >= 3.0f)
				{
					//random number 0 or 1
					int rnd = rand() % 2;
					//if modulo 2 returns a value (remainder)
					//then select to jump up (else down)
					if (rnd % 2)
					{
						mJumpUpFlag = true;
					}
					else
					{
						if (mOnOnewayPlatformIs)
							mJumpDownFlag = true;
						else
							mMoveTimer.Reset();
					}
				}

				//if we are on a one way platform and are jumping down
				if (mJumpDownFlag)
				{
					//move us below the threshold if one way
					if (mOnOnewayPlatformIs)
						mPosition.y -= mOneWayPlatformThreshold;
				}
				//or are we jumping now?
				else if (mJumpUpFlag)
				{
					mSpeed.y = mJumpSpeed;
					mMoveState = BOSS_JUMP_MOVESTATE;
				}

				break;
			}
			//jumping state
			case BOSS_JUMP_MOVESTATE:
			{
				//gravity / falling values
				mSpeed.y += mGravity * deltaTime;
				mSpeed.y = max(mSpeed.y, mMaxFallSpeed);
				//determine direction to face based on player x position
				if (playerPos.x < mPosition.x)
				{
					isFacingLeft = true;
				}
				else
				{
					isFacingLeft = false;
				}

				//Change sprites if firing --------------------------------------------
				if (mActionState == BOSS_FIRING_ACTIONSTATE)
				{
					mCurrentSprite = mBossJumpFireSprite;
				}
				else
				{
					mCurrentSprite = mBossJumpSprite;
				}

				//if we hit the ground
				if (mOnGroundIs)
				{
					//reset timer, state and flags
					mMoveState = BOSS_STAND_MOVESTATE;
					mSpeed.y = 0.0f;
					mMoveTimer.Reset();
					mJumpUpFlag = false;
					mJumpDownFlag = false;
				}
			}
			break;
			}
			//life state checks
			//if lifestate is ok, no color lerp
			if (mLifeState == ENEMY_OK_LIFESTATE)
			{
				mColorLerpValue = 0.0f;
				mCurrentSprite->mColorLerpValue = mColorLerpValue;
				mCurrentSprite->mAltColorValue = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			}
			//if we are hurt...
			if (mLifeState == ENEMY_HURT_LIFESTATE)
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
					mLifeState = ENEMY_OK_LIFESTATE;
					mLifeTimer.Reset();
				}
			}
		}
		break;

		}
		//firing checks - tick timer
		mFireTimer.Tick();
		//if we have been non-firing for a second, switch state
		if (mFireTimer.TotalTime() >= 0.6f &&
			mActionState == BOSS_NONFIRING_ACTIONSTATE)
		{
			mActionState = BOSS_FIRING_ACTIONSTATE;
			//update the frames based on direction facing
			if (isFacingLeft)
			{
				mBossJumpFireSprite->SetCurrentFrame(3);
				mBossStandFireSprite->SetCurrentFrame(3);
			}
			else
			{
				mBossJumpFireSprite->SetCurrentFrame(0);
				mBossStandFireSprite->SetCurrentFrame(0);
			}
			//reset timer
			mFireTimer.Reset();
		}

		if (mActionState == BOSS_FIRING_ACTIONSTATE)
		{
			//if the firing stat is set update fire offset from center
			//and animation directions
			if (isFacingLeft)
			{
				mFireOffset.x = -5.0f; mFireOffset.y = 20.0f;
				mBossJumpFireSprite->mAnimationDirection = -16;
				mBossStandFireSprite->mAnimationDirection = -16;
			}
			else
			{
				mFireOffset.x = 0.0f; mFireOffset.y = 20.0f;
				mBossJumpFireSprite->mAnimationDirection = 16;
				mBossStandFireSprite->mAnimationDirection = 16;
			}
		}

		//if 0.2 seconds has passed in the firing state
		if (mFireTimer.TotalTime() >= 0.2f &&
			mActionState == BOSS_FIRING_ACTIONSTATE)
		{
			//set flag to fire
			mToFireFlag = true;
			//set state to non-firing
			mActionState = BOSS_NONFIRING_ACTIONSTATE;
			//set fire type
			mFireType = FIRE_SYRINGE;
			//reset timer
			mFireTimer.Reset();
			//if player below the enemy, fire down diagonal towards them
			//(direction is unit vector - precalculated)
			if (mPosition.y - mAABB.halfSize.y > playerPos.y)
			{
				mFireDirection.y = -0.7f;
				if (isFacingLeft)
					mFireDirection.x = -0.7f;
				else
					mFireDirection.x = 0.7f;
			}
			//if player above the enemy, fire up diagonal towards them
			//(direction is unit vector - precalculated)
			else if (mPosition.y + mAABB.halfSize.y < playerPos.y)
			{
				mFireDirection.y = +0.7f;
				if (isFacingLeft)
					mFireDirection.x = -0.7f;
				else
					mFireDirection.x = 0.7f;
			}
			else
			//otheriwse fire straight in the player direction
			{
				mFireDirection.y = 0.0f;
				if (isFacingLeft)
					mFireDirection.x = -1.0f;
				else
					mFireDirection.x = 1.0f;
			}

		}

		UpdatePhysics(deltaTime, map);

		mWeakHitBox.center.x = mPosition.x; mWeakHitBox.center.y = mPosition.y;

		//do any sound FX on changes with interaction here

		//current sprite updates
		mCurrentSprite->Update(deltaTime);
		if (isFacingLeft)
		{
			mCurrentSprite->mFlipValue = -1.0f;
		}
		else
		{
			mCurrentSprite->mFlipValue = 1.0f;
		}

		mCurrentSprite->mPosition.x = mPosition.x;
		mCurrentSprite->mPosition.y = mPosition.y;
	}
}

bool PandaEngine::Boss::TakeDamage(int dmg, AABB2D fireBox)
{
	//(could play sounds here using the audio manager pointer)

	//if fire AABB intersects with the weak hitbox
	if (mWeakHitBox.Overlaps(fireBox))
	{
		//deduct dmg
		mHealth -= dmg;
		//if health run out
		if (mHealth <= 0)
		{
			//set to die lifestate and flag for removal
			mLifeState = ENEMY_DIE_LIFESTATE;
			mRemoveFlag = true;
		}
		else
		{
			//set to hurt lifestate
			mLifeState = ENEMY_HURT_LIFESTATE;
		}
		//adjust lerp value and reset timer
		mColorLerpValue = 0.5f;
		mLifeTimer.Reset();
		return true;
	}
	else
	{
		return false;
	}
}

//reset timers
void PandaEngine::Boss::ResetTimers()
{
	mMoveTimer.Reset();
}

