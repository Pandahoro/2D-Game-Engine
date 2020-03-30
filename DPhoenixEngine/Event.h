#pragma once

#include "XboxPad.h"

namespace DPhoenix
{
	class IEvent
	{								//event interface
	protected:
		int mId;					//unique id for event
	public:
		IEvent();					//interface constructor
		virtual ~IEvent() {}		//interface destructor
		int GetID() { return mId; }	//get ID
	};

	enum EventTypes
	{									//enumerator for event type										
		EVENT_KEYPRESS,					//when updating event types
		EVENT_KEYRELEASE,				//this can be updated when 
		EVENT_MOUSECLICK,				//new events need to be fired
		EVENT_MOUSEMOTION,
		EVENT_MOUSEWHEEL,
		EVENT_MOUSEMOVE,
		EVENT_XBOX_INPUT
	};

	//All events implement the interface

	class KeyPressEvent : public IEvent
	{											//Key press	
	public:
		int mKeycode;							//DirectInput code
		KeyPressEvent(int key);					//raise event with code
	};

	class KeyReleaseEvent : public IEvent
	{											//Key release
	public:
		int mKeycode;							//DirectInput code
		KeyReleaseEvent(int key);				//raise event with code
	};

	class MouseClickEvent : public IEvent
	{											//Mouse Click
	public:
		int mButton;							//button pressed
		int mPosX, mPosY;						//x, y pos
		MouseClickEvent(int btn, int x, int y);	//raise event with button
	};

	class MouseMotionEvent : public IEvent
	{											//Mouse delta position
	public:
		int mDeltaX, mDeltaY;					//delta x, y
		MouseMotionEvent(int dx, int dy);		//raise event with data
	};

	class MouseWheelEvent : public IEvent
	{											//Mouse wheel event
	public:
		int mWheel;								//wheel value
		MouseWheelEvent(int wheel);				//raise event with value
	};

	class MouseMoveEvent : public IEvent
	{											//Mouse position event
	public:
		int mPosX, mPosY;						//x, y pos
		MouseMoveEvent(int px, int py);			//raise event with data
	};

	class XBoxEvent : public IEvent
	{											//Xbox input event
	public:
		XINPUT_STATE mPadState;					//pad state desc
		XboxPad* mPadPointer;					//pointer to the pad
		XBoxEvent(XINPUT_STATE padState,
			XboxPad* padPtr);					//raise event with data
	};

};

