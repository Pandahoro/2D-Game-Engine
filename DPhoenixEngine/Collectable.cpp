#include "Collectable.h"

DPhoenix::Collectable::Collectable(TextureMgr * mTexMgr, ID3D11Device * md3dDevice,
	XMFLOAT2 position, CollectableTypes type, bool isOnTimer)
{
	//initialise values
	mCollectableType = type;
	mRemoveFlag = false;
	mPosition = position;
	//load sprite based on collectable type
	switch (type)
	{
		case COLLECTABLE_RESEARCH:
		{
			mCollectibleSprite = new DPhoenix::Sprite();
			mCollectibleSprite->Load("Textures\\ObjectAssets\\Research.png", mTexMgr, 30.0f, 30.0f, md3dDevice);
			mCollectibleSprite->SetCurrentFrame(0);	mCollectibleSprite->mAnimationColumns = 4;
			mCollectibleSprite->mAnimationDirection = 6;	mCollectibleSprite->SetAnimationRange(0, 3);
			mCollectibleSprite->SetScale(1.0f);	mCollectibleSprite->mFlipValue = 1.0f;
		}
		break;
		case COLLECTABLE_COFFEE:
		{
			mCollectibleSprite = new DPhoenix::Sprite();
			mCollectibleSprite->Load("Textures\\ObjectAssets\\Coffee.png", mTexMgr, 30.0f, 60.0f, md3dDevice);
			mCollectibleSprite->SetCurrentFrame(0);	mCollectibleSprite->mAnimationColumns = 4;
			mCollectibleSprite->mAnimationDirection = 6;	mCollectibleSprite->SetAnimationRange(0, 3);
			mCollectibleSprite->SetScale(1.0f);	mCollectibleSprite->mFlipValue = 1.0f;
		}
		break;
	}

	//set bounding box center and size to match srpite bounds
	mAABB.center = mPosition;
	mAABBOffset = XMFLOAT2(0.0f, 0.0f);
	mAABB.halfSize.x = mCollectibleSprite->mSize.x / 2;
	mAABB.halfSize.y = mCollectibleSprite->mSize.y / 2;

	//set sprite position
	mCollectibleSprite->mPosition.x = mPosition.x;
	mCollectibleSprite->mPosition.y = mPosition.y;
	//set timer value
	mIsOnTimer = isOnTimer;
	//if timer set, reset for use
	if (mIsOnTimer)
	{
		mLifeTimer.Start();
		mLifetime = 1.0f;
	}

}
//update logic
void DPhoenix::Collectable::Update(float dt)
{
	//update sprite animation
	mCollectibleSprite->Update(dt);
	//if on timer...
	if (mIsOnTimer)
	{	//update timer
		mLifeTimer.Tick();
		//if total time greater than lifetime set remove flag
		if (mLifeTimer.TotalTime() >= mLifetime)
		{
			mRemoveFlag = true;
		}
	}
}
