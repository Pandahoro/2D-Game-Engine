#pragma once

#include "D3DUtil.h"

namespace DPhoenix
{
	//defined in it's own file but ensures can be used for Sprite
	class TextureMgr;

	class Sprite
	{
	protected:
		//vertex and index buffers for the quad / sprite
		ID3D11Buffer* mSpriteVB;
		ID3D11Buffer* mSpriteIB;
		//texture (shader resource view)
		ID3D11ShaderResourceView* mSpriteTexSRV;

	public:
		XMFLOAT3 mPosition;			//position
		XMFLOAT3 mVelocity;			//velocity
		XMFLOAT3 mSize;				//size
		int mAnimationColumns;		//anim cols
		float mCurrentFrame;		//anim frame
		int mFirstFrame;			//first frame
		int mLastFrame;				//last frame
		int mAnimationDirection;	//anim dir
		double mRotation;			//rotation
		XMFLOAT3 mScale;			//scale
		XMFLOAT3 mPivotPoint;		//origin
		XMFLOAT2 mInitialPosition;	//initial position (if required to respawn)
		XMFLOAT2 mParallax;			//a parralax value for different scrolling speeds 
									//some funky maths go with this for 2D games

		float mFlipValue;			//for mirroring the texture / flipping it
		float mOpacityValue;		//opacity (inv. transpaency) 

		float mColorLerpValue;		//for color tints - lerp value
		XMFLOAT4 mAltColorValue;	//alternative colour to lerp to

		//constructor / destructor
		Sprite();
		virtual ~Sprite();

		//load image
		bool Load(std::string filename, TextureMgr* mTexMgr,
			float width, float height, ID3D11Device* md3dDevice);

		//managed functions
		void Update(float deltaTime);
		void Render(ID3D11DeviceContext* dc);		//draw sprite to screen
		
		//this is used to generate the matrices for rendering the sprite
		XMMATRIX CalculateTransforms(XMFLOAT3 screenScale, bool isHUD = false);

		//this provides the sprite image for the shader as an SRV
		ID3D11ShaderResourceView* GetShaderResourceView()
		{
			return mSpriteTexSRV;
		}

		//current animation frame
		int GetCurrentFrame() 
		{
			//we need to round down or up to avoid animation glitching
			//when animating in different directions
			//'play forwards' / 'play backwards'
			if (mAnimationDirection < 0)
			{
				if (mCurrentFrame > mLastFrame)
					mCurrentFrame = mLastFrame;
				return (int)ceil(mCurrentFrame);
			}
			else
			{
				if (mCurrentFrame < mFirstFrame)
					mCurrentFrame = mFirstFrame;
				return (int)floor(mCurrentFrame);
			}
		}

		//setter for current frame
		void SetCurrentFrame(int value) { mCurrentFrame = (float)value; }

		//sets the first and last frame for animating
		void SetAnimationRange(int first, int last)
		{
			mFirstFrame = first;
			mLastFrame = last;
		}

		//setters for scaling the sprite (and used in CalculateTransforms)
		void SetScale(double horiz, double vert)
		{
			mScale.x = horiz;
			mScale.y = vert;
		}
		void SetScale(double scale) { SetScale(scale, scale); }

		//returns boundary of sprite as RECT structure
		RECT GetBounds();

	}; //class
};

