#include "Fire.h"

PandaEngine::Fire::Fire(TextureMgr* mTexMgr, ID3D11Device* md3dDevice, XMFLOAT2 position, 
	XMFLOAT2 direction, FireTypes fireType, AudioMgr* audioMgr)
{
	//init vars based on parameters
	mAudioMgr = audioMgr;
	mFireType = fireType;

	//initialise SUVAT vars
	mGravity = -1500.0f;
	isSuvatFlag = false;
	mSuvatInitialVelocity = XMFLOAT2(0.0f, 0.0f);
	mTimeElapsed = 0.0f;
	
	//check the firetype and initialise accordingly
	switch (mFireType)
	{
		case FIRE_PLAYER:	//'knowledge' fireballs from the player
		{
			//spritesheet init
			mFireSprite = new PandaEngine::Sprite();
			mFireSprite->Load("Textures\\ObjectAssets\\KnowledgeFire.png", mTexMgr, 20.0f, 20.0f, md3dDevice);
			mFireSprite->SetCurrentFrame(0);	mFireSprite->mAnimationColumns = 8;
			mFireSprite->mAnimationDirection = 16;	mFireSprite->SetAnimationRange(0, 7);
			mFireSprite->SetScale(1.0f);	mFireSprite->mFlipValue = 1.0f;

			mIsFriendlyFire = true;	//set 'friendly fire'

			mSpeed = 600.0f;		//set speed (can be tweaked, or additionally sent as param)
			
			//set bounding box properties
			mAABB.halfSize.x = 10.0f; mAABB.halfSize.y = 10.0f;
		}
		break;
		case FIRE_CHEMTRAILS:
		{
			//spritesheet init
			mFireSprite = new PandaEngine::Sprite();
			mFireSprite->Load("Textures\\Enemies\\BatStuff.png", mTexMgr, 20.0f, 20.0f, md3dDevice);
			mFireSprite->SetCurrentFrame(0);	mFireSprite->mAnimationColumns = 8;
			mFireSprite->mAnimationDirection = 12;	mFireSprite->SetAnimationRange(0, 7);
			mFireSprite->SetScale(3.0f);	mFireSprite->mFlipValue = 1.0f;

			mIsFriendlyFire = false;	//not firendly fire

			mSpeed = 200.0f;			//speed (descends slowly - player fire quicker)

			//set bounding box properties
			mAABB.halfSize.x = 25.0f; mAABB.halfSize.y = 25.0f;
		}
		break;
		case FIRE_SYRINGE:
		{
			//spritesheet init
			mFireSprite = new PandaEngine::Sprite();
			mFireSprite->Load("Textures\\Enemies\\brick.png", mTexMgr, 20.0f, 20.0f, md3dDevice);
			mFireSprite->SetCurrentFrame(0);	mFireSprite->mAnimationColumns = 8;
			mFireSprite->mAnimationDirection = 12;	mFireSprite->SetAnimationRange(0, 7);
			mFireSprite->SetScale(2.0f);	mFireSprite->mFlipValue = 1.0f;

			mIsFriendlyFire = false;	//not firendly fire

			mSpeed = 600.0f;			//speed (quick projectile)
			//set bounding box properties
			mAABB.halfSize.x = 8.0f; mAABB.halfSize.y = 8.0f;
			//if not up / down on y
			if (direction.y != 0.0f)
			{
				//set as suvat projectile
				isSuvatFlag = true;
				//pi used to calculate angle properties
				double pi = 3.14159265358979323846;
				//calculate launch angle based on direction
				double angleInRadians = std::atan2(direction.y, direction.x);
				double angleInDegrees = (angleInRadians / pi) * 180.0;
				//set initial position (to calculate arc from)
				mInitialPos = position;
				//calculate initial velocity (u) on x and y using cos / sin 
				mSuvatInitialVelocity.x = std::cos(angleInRadians) * mSpeed;
				mSuvatInitialVelocity.y = std::sin(angleInRadians) * mSpeed;
			}

		}
		break;

	}

	//initialise vars
	mPosition = position;
	mDirection = direction;
	mAABB.center = mPosition;
}

//update logic
void PandaEngine::Fire::Update(float deltaTime)
{

	//update overall time elapsed (used for SUVAT)
	mTimeElapsed += deltaTime;
	//if not suvat projectile
	if (!isSuvatFlag)
	{
		//adjust position by direction, speed, delta time
		mPosition.x += mDirection.x * mSpeed * deltaTime;
		mPosition.y += mDirection.y * mSpeed * deltaTime;
	}
	else
	{
		//x position is speed * time = (s = 0.5*(u + v)*t)
		mPosition.x = mInitialPos.x + mSuvatInitialVelocity.x * mTimeElapsed;
		//y position is (s = u*t + 0.5*a*t^2)
		mPosition.y = mInitialPos.y + ((mSuvatInitialVelocity.y * mTimeElapsed) + 
			(0.5 * mGravity * (mTimeElapsed * mTimeElapsed)));
	}

	//update AABB position
	mAABB.center = mPosition;
	//update sprite position
	mFireSprite->mPosition.x = mPosition.x;
	mFireSprite->mPosition.y = mPosition.y;
	mFireSprite->Update(deltaTime);
}

