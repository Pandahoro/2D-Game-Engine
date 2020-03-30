#include "Angel.h"

DPhoenix::Angel::Angel(TextureMgr * mTexMgr, ID3D11Device * md3dDevice, 
	XMFLOAT2 position, XMFLOAT2 direction)
{
	//spritesheet properties
	mAngelSprite = new DPhoenix::Sprite();
	mAngelSprite->Load("Textures\\Enemies\\Angel.png", mTexMgr, 50.0f, 50.0f, md3dDevice);
	mAngelSprite->SetCurrentFrame(0);	mAngelSprite->mAnimationColumns = 4;
	mAngelSprite->mAnimationDirection = 10;	mAngelSprite->SetAnimationRange(0, 3);
	mAngelSprite->SetScale(1.0f);	mAngelSprite->mFlipValue = 1.0f;

	//set position and direction
	mPosition = position;
	mDirection = direction;
	//speed and acceleration values hard coded 
	mSpeed = 100.0f;
	mAcceleration = 200.0f;
	//update bounding box position
	mAABB.center = mPosition;
}

//update logic
void DPhoenix::Angel::Update(float deltaTime)
{
	//adjust speed by acceleration value (* dt)
	mSpeed += mAcceleration * deltaTime;

	//update position by direction and speed (*dt)
	mPosition.x += mDirection.x * mSpeed * deltaTime;
	mPosition.y += mDirection.y * mSpeed * deltaTime;

	//update AABB position 
	mAABB.center = mPosition;
	//update sprite properties and animate
	mAngelSprite->mPosition.x = mPosition.x;
	mAngelSprite->mPosition.y = mPosition.y;
	mAngelSprite->Update(deltaTime);
}

