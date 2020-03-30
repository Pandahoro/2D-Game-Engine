#include "D3DUtil.h"

namespace DPhoenix
{
	//Event constructor - resets id
	IEvent::IEvent()
	{
		mId = 0;
	}

	KeyPressEvent::KeyPressEvent(int key)
	{
		mId = EVENT_KEYPRESS;		//set enum
		mKeycode = key;				//set key
	}

	KeyReleaseEvent::KeyReleaseEvent(int key)
	{
		mId = EVENT_KEYRELEASE;		//set enum
		mKeycode = key;				//set key
	}

	MouseClickEvent::MouseClickEvent(int btn, int x, int y)
	{
		mId = EVENT_MOUSECLICK;		//set enum
		mButton = btn;				//set button
		mPosX = x; mPosY = y;
	}

	MouseMotionEvent::MouseMotionEvent(int dx, int dy)
	{
		mId = EVENT_MOUSEMOTION;	//set enum
		mDeltaX = dx;				//set delta x
		mDeltaY = dy;				//set delta y
	}

	MouseWheelEvent::MouseWheelEvent(int whl)
	{
		mId = EVENT_MOUSEWHEEL;		//set enum
		mWheel = whl;				//set wheel val
	}

	MouseMoveEvent::MouseMoveEvent(int px, int py)
	{
		mId = EVENT_MOUSEMOVE;		//set enum
		mPosX = px;					//set x pos
		mPosY = py;					//set y pos
	}

	XBoxEvent::XBoxEvent(XINPUT_STATE padState, XboxPad* padPtr)
	{
		mId = EVENT_XBOX_INPUT;		//set enum
		mPadState = padState;		//set pad state desc
		mPadPointer = padPtr;		//set pad pointer
	}
};

