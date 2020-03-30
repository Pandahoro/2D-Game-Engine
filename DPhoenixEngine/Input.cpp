#include "D3DUtil.h"

namespace DPhoenix
{
	Input::Input(HWND hwnd)
	{
		//save window handle
		mWindow = hwnd;

		//create DirectInput object
		DirectInput8Create(
			GetModuleHandle(NULL),		//get this app handle		
			DIRECTINPUT_VERSION,		//DI version
			IID_IDirectInput8,			//Unique ID
			(void**)&mDi,				//pointer to di object
			NULL						//NULL
		);

		//initialise keyboard
		mDi->CreateDevice(
			GUID_SysKeyboard,			//Unique ID
			&mKeyboard,					//keyboard object
			NULL						//NULL
		);

		//keyboard data format (standard DI)
		mKeyboard->SetDataFormat(&c_dfDIKeyboard);

		//for this window foreground means
		//use when in FOREGROUND and give up access
		//thereafter; NONEXCLUSIVE
		//means it doesn't stop other apps using it
		mKeyboard->SetCooperativeLevel(
			mWindow,
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
		);

		//Get keyboard control
		mKeyboard->Acquire();

		//clear key array
		memset(mKeyState, 0, 256);

		//initialise mouse
		mDi->CreateDevice(
			GUID_SysMouse,				//Unique ID
			&mMouse,					//mouse object
			NULL						//NULL
		);

		//mouse data format (standard DI)
		mMouse->SetDataFormat(&c_dfDIMouse);

		//for this window foreground means
		//use when in FOREGROUND and give up access
		//thereafter; NONEXCLUSIVE
		//means it doesn't stop other apps using it
		mMouse->SetCooperativeLevel(
			mWindow,
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE
		);

		//get mouse control
		mMouse->Acquire();

	}

	Input::~Input()
	{
		mMouse->Release();		//release mouse object
		mKeyboard->Release();	//release keyboard object
		mDi->Release();			//release direct input object
	}

	void Input::Update()
	{
		//poll state of the keyboard
		mKeyboard->Poll();

		//get state of keys - buffer and pointer to storage
		if (!SUCCEEDED(
			mKeyboard->GetDeviceState(256, (LPVOID)&mKeyState))
			)
		{
			//keyboard device lost, try to re-acquire
			mKeyboard->Acquire();
		}

		//poll state of the mouse
		mMouse->Poll();

		//get state of mouse - buffer and pointer to storage
		if (!SUCCEEDED(
			mMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mMouseState))
			)
		{
			//mouse device lost, try to re-acquire
			mMouse->Acquire();
		}

		//get mouse position on screen (not DirectInput)
		GetCursorPos(&mPosition);

		//get position relative to window origin
		ScreenToClient(mWindow, &mPosition);
	}

	int Input::GetMouseButton(char button)
	{
		//get whether button is pressed
		return(mMouseState.rgbButtons[button] & 0x80);
	}
};

