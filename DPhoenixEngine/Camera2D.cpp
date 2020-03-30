#include "Camera2D.h"

void DPhoenix::Camera2D::Update(float deltaTime, int clientWidth, int clientHeight)
{
	//set the window dimensions
	mClientWidth = clientWidth;
	mClientHeight = clientHeight;
	//initialise speed
	mSpeed.x = 0.0f; mSpeed.y = 0.0f;

	//update speed values based on input 
	if (KeyState(CAMERA2D_LEFT_INPUT))
		mSpeed.x = -500.0f;
	else if (KeyState(CAMERA2D_RIGHT_INPUT))
		mSpeed.x = 500.0f;

	if (KeyState(CAMERA2D_DOWN_INPUT))
		mSpeed.y = -500.0f;
	else if (KeyState(CAMERA2D_UP_INPUT))
		mSpeed.y = 500.0f;

	//adjust camera position by speed * dt
	mPosition.x += mSpeed.x * deltaTime;
	mPosition.y += mSpeed.y * deltaTime;

	//bound camera position by limits set
	if (mPosition.x < mLeftLimit)
		mPosition.x = mLeftLimit;

	if (mPosition.x > mRightLimit)
		mPosition.x = mRightLimit;

	if (mPosition.y < mBottomLimit)
		mPosition.y = mBottomLimit;

	if (mPosition.y > mTopLimit)
		mPosition.y = mTopLimit;

	//update the previous inputs
	UpdatePrevInputs();
}

XMMATRIX DPhoenix::Camera2D::GetViewProj(XMFLOAT2 parallax)
{
	//camera translation matrix needs to be inverted
	//multiplying by a parallax value adjusts scroll speed for bg / fg assets
	XMMATRIX mView = XMMatrixTranslation(-mPosition.x * parallax.x, 
		-mPosition.y * parallax.y, 0.0f);

	//orthographic, left handed co-ordinates
	XMMATRIX mProjection = XMMatrixOrthographicLH(
		(float)mClientWidth,	//width of viewport
		(float)mClientHeight,	//height of viewport
		0.1f,					//nearplane (Z)
		100.0f					//far plane (Z)
	);

	//concatenate matrices for output to shader
	XMMATRIX viewProj = mView * mProjection;

	return viewProj;
}

//has key been released?
bool DPhoenix::Camera2D::Released(Camera2DInputs key)
{
	return (!mInputs[(int)key] && mPrevInputs[(int)key]);
}

//is an input active given the key?
bool DPhoenix::Camera2D::KeyState(Camera2DInputs key)
{
	return (mInputs[(int)key]);
}

//has an input just been pressed given the key?
bool DPhoenix::Camera2D::Pressed(Camera2DInputs key)
{
	return (mInputs[(int)key] && !mPrevInputs[(int)key]);
}

//update the previous inputs array
void DPhoenix::Camera2D::UpdatePrevInputs()
{
	for (int i = 0; i < CAMERA2D_MAX_INPUT; i++)
	{
		mPrevInputs[i] = mInputs[i];
		//flush for new inputs on next frame
		mInputs[i] = false;
	}
}

