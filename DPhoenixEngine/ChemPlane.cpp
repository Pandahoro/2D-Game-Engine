#include "ChemPlane.h"

PandaEngine::ChemPlane::ChemPlane(TextureMgr* mTexMgr, ID3D11Device* md3dDevice, 
	XMFLOAT2 position, AudioMgr* audioMgr)
{
	//audio manager pointer
	mAudioMgr = audioMgr;

	//spritsheet initialisation
	mChemPlaneSprite = new PandaEngine::Sprite();
	mChemPlaneSprite->Load("Textures\\Enemies\\EyeFlight.png", mTexMgr, 150.0f, 150.0f, md3dDevice);
	mChemPlaneSprite->SetCurrentFrame(0);	mChemPlaneSprite->mAnimationColumns = 8;
	mChemPlaneSprite->mAnimationDirection = 10;	mChemPlaneSprite->SetAnimationRange(0, 7);
	mChemPlaneSprite->SetScale(2.0f);	mChemPlaneSprite->mFlipValue = 1.0f;
	//set pointer to current sprite
	mCurrentSprite = mChemPlaneSprite;

	//initially facing left
	isFacingLeft = true;

	//flags
	mIsActive = false;			//not active till onscreen
	mToFireFlag = false;		//set only when firing
	mRemoveFlag = false;		//set when dead /. marked for removal
	isHazard = false;			//enemy not a collison hazard

	//health values
	mHealth = 100; mMaxHealth = 100;

	//state initialisations
	mEnemyType = ENEMY_CHEMPLANE;
	mMoveState = CHEM_HOVER_MOVESTATE;
	mLifeState = ENEMY_OK_LIFESTATE;

	//parent class initialisations
	mInitialSpawnPos = position;
	mPosition = position;
	//tile collison box (need to be initialised, but not used for this enemy)
	mAABB.center.x = 0.0f; mAABB.center.y = 0.0f;
	mAABB.halfSize.x = 40.0f; mAABB.halfSize.y = 15.0f;
	mAABBOffset.x = 0.0f; mAABBOffset.y = 0.0f;

	//main hit boxes
	mWeakHitBox.center.x = 0.0f; mWeakHitBox.center.y = 0.0f;
	mWeakHitBox.halfSize.x = 40.0f; mWeakHitBox.halfSize.y = 15.0f;
	mWeakHitBBOffset.x = 0.0f; mWeakHitBBOffset.y = 0.0f;

	//flags for collison (need to be initialised, but not used for this enemy)
	mPushRightWallJust = false; mPushRightWallIs = false;
	mPushLeftWallJust = false; mPushLeftWallIs = false;
	mOnGroundJust = false; mOnGroundIs = false;
	mAtCeilingJust = false; mAtCeilingIs = false;

	//flags set so tile collision not checked for this enemy
	mIsTileCollider = false;
	mIsWallCollider = false;

	//for one way platforms (not necessary but init for good practice)
	mOneWayPlatformThreshold = 2.0f;
	mOnOnewayPlatformIs = false;

	//begin timers
	mMoveTimer.Start();
	mFireTimer.Start();

	//firing vars initialisation
	mFireOffset.x = 0.0f; mFireOffset.y = 0.0f;
	mFireDirection.x = 0.0f; mFireDirection.y = -1.0f;
	mFireType = FIRE_CHEMTRAILS;

}
//takes in player position to determine movement
void PandaEngine::ChemPlane::EnemyUpdate(float deltaTime, PandaEngine::Map * map, 
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
			//f hovering
			case CHEM_HOVER_MOVESTATE:
			{
				//no movement on X if hovering in place
				mSpeed.x = 0.0f;
				//determine direction to face based on player position
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
				//adjust speed to hover up / down in direction of player
				if (playerPos.y >= mPosition.y)
				{
					mSpeed.y = 10.0f;
				}
				else
				{
					mSpeed.y = -10.0f;
				}
				//tick the move timer
				mMoveTimer.Tick();

				//should prob const this value / no magic numbers etc.
				//after 3 seconds
				if (mMoveTimer.TotalTime() >= 3.0f)
				{
					//reset timer and switch to move state
					mMoveTimer.Reset();
					mMoveState = CHEM_ZOOM_MOVESTATE;

					//speed up animation
					mCurrentSprite->mAnimationDirection = 12;
					//go fast horizontally in direction of the player
					if (playerPos.x >= mPosition.x)
					{
						mSpeed.x = 500.0f;
					}
					else
					{
						mSpeed.x = -500.0f;
					}
					//slightly slower on the vertical axis ('torpedo' like)
					if (playerPos.y >= mPosition.y)
					{
						mSpeed.y = 50.0f;
					}
					else
					{
						mSpeed.y = -50.0f;
					}
				}
			}
			break;
			//if in zoom movestate
			case CHEM_ZOOM_MOVESTATE:
			{
				//tick the timer
				mMoveTimer.Tick();

				//should prob const this value / no magic numbers etc.
				//after 2 seconds
				if (mMoveTimer.TotalTime() >= 2.0f)
				{
					//reset timer and update movestate and animation speed
					mMoveTimer.Reset();
					mMoveState = CHEM_HOVER_MOVESTATE;
					mCurrentSprite->mAnimationDirection = 8;
				}
				//tick the fire timer - chemtrails only spread when zooming
				mFireTimer.Tick();
				//after a second set the fire flag and reset timer
				if (mFireTimer.TotalTime() >= 1.0f)
				{
					mToFireFlag = true;					
					mFireTimer.Reset();
				}

				break;
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

		UpdatePhysics(deltaTime, map);

		mWeakHitBox.center.x = mPosition.x; mWeakHitBox.center.y = mPosition.y;

		//do any sound FX on changes with interaction here

		//current sprite updates
		mCurrentSprite->Update(deltaTime);
		if (isFacingLeft)
			mCurrentSprite->mFlipValue = -1.0f;
		else
			mCurrentSprite->mFlipValue = 1.0f;

		mCurrentSprite->mPosition.x = mPosition.x; 
		mCurrentSprite->mPosition.y = mPosition.y;
	}
}

bool PandaEngine::ChemPlane::TakeDamage(int dmg, AABB2D fireBox)
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
void PandaEngine::ChemPlane::ResetTimers()
{
	mMoveTimer.Reset();
}
