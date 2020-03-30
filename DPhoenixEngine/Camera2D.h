#pragma once
#include "D3DUtil.h"

namespace PandaEngine
{
	//inputs for testing bounds
	enum Camera2DInputs
	{
		CAMERA2D_LEFT_INPUT,
		CAMERA2D_RIGHT_INPUT,
		CAMERA2D_UP_INPUT,
		CAMERA2D_DOWN_INPUT,
		CAMERA2D_MAX_INPUT	//used for indexing arrays - not 'safe'
							//if we do not use default numbering
	};

	class Camera2D
	{
	public:
		//view / projection matrices
		XMMATRIX mView;
		XMMATRIX mProjection;
		//position and camera speed
		XMFLOAT2 mPosition;
		XMFLOAT2 mSpeed;

		//bounds for camera (so we can fit it to the map limits)
		float mLeftLimit;
		float mRightLimit;
		float mTopLimit;
		float mBottomLimit;

		//dimesnions of client window
		int mClientWidth;
		int mClientHeight;

		//arrays for input handling
		bool mInputs[CAMERA2D_MAX_INPUT];
		bool mPrevInputs[CAMERA2D_MAX_INPUT];

		//constructor / destructor
		Camera2D() { };
		~Camera2D() { };

		//update camera logic
		void Update(float deltaTime, int clientWidth, int clientHeight);
		//get the view*projection matrix for the shader
		XMMATRIX GetViewProj(XMFLOAT2 parallax);

		//key handling
		bool Released(Camera2DInputs key);
		bool KeyState(Camera2DInputs key);
		bool Pressed(Camera2DInputs key);

		void UpdatePrevInputs();
	};
}

