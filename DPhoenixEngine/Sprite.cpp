#include "D3DUtil.h"

namespace DPhoenix
{
	//constructor - initialise vars - render type
	Sprite::Sprite()
	{
		//basic properties - pointers
		mSpriteVB = NULL;
		mSpriteIB = NULL;
		mSpriteTexSRV = NULL;

		mPosition.x = 0.0f; mPosition.y = 0.0f; mPosition.z = 0.0f;		//position
		mInitialPosition.x = 0.0f; mInitialPosition.y = 0.0f;			//initial positon (e.g. spawn point)
		mVelocity.x = 0.0f; mVelocity.y = 0.0f; mVelocity.z = 0.0f;		//velocity
		mSize.x = 1.0f; mSize.y = 1.0f; mSize.z = 1.0f;					//size
		mAnimationColumns = 1;											//anim cols
		mCurrentFrame = 0;												//anim frame
		mFirstFrame = 0;												//first frame
		mLastFrame = 0;													//last frame
		mAnimationDirection = 1;										//anim dir
		mRotation = 0.0f;												//rotation
		mScale.x = 1.0f; mScale.y = 1.0f; mScale.z = 1.0f;				//scale
		mPivotPoint.x = 0.0f; mPivotPoint.y = 0.0f;						//origin
		mPivotPoint.z = 0.0f;											
		
		mFlipValue = 1.0f;												//face forwards
		mOpacityValue = 1.0f;											//opaque

		mColorLerpValue = 0.0f;											//lerp value between colors
		mAltColorValue = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);				//alternative color to lerp to
		
		mParallax.x = 1.0f;												//standard scrolling
		mParallax.y = 1.0f;
	}

	//destructor
	Sprite::~Sprite()
	{
		ReleaseCOM(mSpriteVB);
		ReleaseCOM(mSpriteIB);
		ReleaseCOM(mSpriteTexSRV);
	}

	//Load image
	bool Sprite::Load(std::string filename, TextureMgr* mTexMgr,
		float width, float height, ID3D11Device* md3dDevice)
	{
		//create new texture from texture manager
		mSpriteTexSRV = mTexMgr->CreateTexture(filename);

		//get the size on x, y from width, height
		mSize.x = width;
		mSize.y = height;

		//calculate half sizes so origin in centre
		float halfWidth = mSize.x / 2;
		float halfHeight = mSize.y / 2;

		//A quad with UV co-ords
		Vertex::SpritePosTex vertices[] =
		{
			//A											//UV1
			{ XMFLOAT3(halfWidth, halfHeight, 1.0f),	XMFLOAT2(1.0f, 0.0f) },
			//B											//UV2
			{ XMFLOAT3(halfWidth, -halfHeight, 1.0f),	XMFLOAT2(1.0f, 1.0f) },
			//C											//UV3
			{ XMFLOAT3(-halfWidth, -halfHeight, 1.0f),	XMFLOAT2(0.0f, 1.0f) },
			//D											//UV4
			{ XMFLOAT3(-halfWidth, halfHeight, 1.0f),	XMFLOAT2(0.0f, 0.0f) }
		};

		//create vertex buffer
		D3D11_BUFFER_DESC vbd;								//desc struct for vertex buffer
		vbd.Usage = D3D11_USAGE_IMMUTABLE;					//usage flags (can't edit but quicker)
		vbd.ByteWidth = sizeof(Vertex::SpritePosTex) * 4;	//four vertices so multiply size by 4
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//bind as vertex buffer
		vbd.CPUAccessFlags = 0;								//CPU access flags
		vbd.MiscFlags = 0;									//Misc flags
		vbd.StructureByteStride = 0;						//this goes pretty deep! Lets leave at 0...
															//create subresource with vertex buffer data
		D3D11_SUBRESOURCE_DATA vinitData;
		//apply vertices array
		vinitData.pSysMem = vertices;
		//create buffer from device (stored in mSpriteVB)
		HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSpriteVB));

		// Create the index buffer
		//(two triangles from teh four vertices)
		UINT indices[] = {
			// front face
			0, 1, 2,
			0, 2, 3,
		};

		//create index buffer subresource
		D3D11_BUFFER_DESC ibd;								//desc struct for vertex buffer
		ibd.Usage = D3D11_USAGE_IMMUTABLE;					//immutable as the box will always be a box
		ibd.ByteWidth = sizeof(UINT) * 6;					//6 indices
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//Bind as index buffer
		ibd.CPUAccessFlags = 0;								//CPU access flags
		ibd.MiscFlags = 0;									//Misc flags
		ibd.StructureByteStride = 0;						//this goes pretty deep! Lets leave at 0...
															//create subresource with index buffer data
		D3D11_SUBRESOURCE_DATA iinitData;
		//apply indices array
		iinitData.pSysMem = indices;
		//create buffer from device (stored in mSpriteIB)
		HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSpriteIB));

		return true;
	}

	//called from rendering methods in game loop
	//takes pointer to device context
	//shader values should be set prioor to this call
	//you may wish to play with the structure of this
	void Sprite::Render(ID3D11DeviceContext* dc)
	{
		//calculate the size of individual data points
		UINT stride = sizeof(Vertex::SpritePosTex);
		UINT offset = 0;

		//set the vertex buffer to the input assembler
		dc->IASetVertexBuffers(0, 1, &mSpriteVB, &stride, &offset);
		//set the index buffer to the input assembler
		dc->IASetIndexBuffer(mSpriteIB, DXGI_FORMAT_R32_UINT, 0);

		//draw the quad using the indices (6 indices total)
		dc->DrawIndexed(6, 0, 0);
	}

	//GET TRANSFORMATION MATRIX
	XMMATRIX Sprite::CalculateTransforms(XMFLOAT3 screenScale, bool isHUD)
	{
		//some parts are commented out through experimentation of setting up matrices
		//you may wish to play around to see the effects

		/* mPivotPoint.x = 0;
		mPivotPoint.y = 0;*/

		//set the scale of the sprite on x and y as an XMVECTOR
		XMVECTOR scaling = XMVectorSet(mScale.x * screenScale.x, mScale.y * screenScale.y, 1.0f, 0.0f);
		//XMVECTOR scaling = XMVectorSet(mScale.x, mScale.y, 1.0f, 0.0f);

		//set the origin of the sprite on x and y as an XMVECTOR
		XMVECTOR origin = XMVectorSet(mPivotPoint.x, mPivotPoint.y, 0.0f, 0.0f);
		/*XMVECTOR translation = XMVectorSet(mPosition.x, mPosition.y, 0.0f, 0.0f);*/

		//the translation is essentially positioning
		XMVECTOR translation;

		//if a HUD sprite then position should be fixed
		if (isHUD)
		{
			translation = XMVectorSet(mPosition.x, mPosition.y, 0.0f, 0.0f);
		}
		else
		{
			//otherwise we'll position relatively to the screen scale
			translation = XMVectorSet(mPosition.x * screenScale.x, 
				mPosition.y * screenScale.y, 0.0f, 0.0f);
		}
		//(the translation isn't perfect and really depends on certain setups)
		//(certainly worthy of exploration to get scalable graphics)
		
		//create the final transformation / world matrix for the sprite from the vectors given
		XMMATRIX T = XMMatrixAffineTransformation2D(scaling,
			origin, mRotation, translation);

		return T;
	}

	//Update logic
	void Sprite::Update(float deltaTime)
	{
		//move sprite by velocity amount
		mPosition.x += mVelocity.x * deltaTime;
		mPosition.y += mVelocity.y * deltaTime;

		//increment animation frame
		mCurrentFrame += (float)((float)mAnimationDirection * deltaTime);

		//keep current frame within bounds
		//ceil / floor used to consider animation directions
		if ((int)ceil(mCurrentFrame) < mFirstFrame)
			mCurrentFrame = (float)mLastFrame;
		if ((int)floor(mCurrentFrame) > mLastFrame)
			mCurrentFrame = (float)mFirstFrame;
	}

	RECT Sprite::GetBounds()
	{
		//calculate bounding box
		RECT rect;
		rect.left = mPosition.x;
		rect.top = mPosition.y;
		rect.right = mPosition.x + mSize.x * mScale.x;
		rect.bottom = mPosition.y + mSize.y * mScale.y;
		return rect;
	}
}

