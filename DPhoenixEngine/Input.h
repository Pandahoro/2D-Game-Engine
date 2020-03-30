#pragma once

#include "D3DUtil.h"

namespace DPhoenix
{
	class Input
	{
	private:
		HWND mWindow;					//window handle
		IDirectInput8 *mDi;				//direct input device
		IDirectInputDevice8 *mKeyboard;	//keyboard
		char mKeyState[256];			//key state array
		IDirectInputDevice8 *mMouse;	//mouse
		DIMOUSESTATE mMouseState;		//mouse state
		POINT mPosition;				//x, y cursor position
	public:
		Input(HWND window);				//constructor
		virtual ~Input();				//destructor
		void Update();					//update

		//getters and setters for values
		//return if key down or not
		char GetKeyState(int key) { return mKeyState[key]; }	
		//get current mouse pos x
		long GetMousePosX() { return mPosition.x; }			
		//get current mouse pos y
		long GetMousePosY() { return mPosition.y; }		
		//get current mouse button
		int GetMouseButton(char button);				
		//how far has the mouse moved since last update on x
		long GetMouseDeltaX() { return mMouseState.lX; }	
		//how far has the mouse moved since last update on y
		long GetMouseDeltaY() { return mMouseState.lY; }		
		//how far has the mouse moved since last update on wheel
		long GetMouseDeltaWheel() { return mMouseState.lZ; }	
	};
};

