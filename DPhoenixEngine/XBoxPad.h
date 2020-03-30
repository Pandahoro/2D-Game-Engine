#pragma once

#include "D3DUtil.h"

namespace DPhoenix
{
	class XboxPad
	{
	private:
		//controller state and number
		XINPUT_STATE mControllerState;
		int mControllerNum;
	public:
		//constructor (and player no.)
		XboxPad(int playerNumber);
		//get input state
		XINPUT_STATE GetState();
		//is connected?
		bool IsConnected();
		//vibrate the pad
		void Vibrate(int leftVal = 0, int rightVal = 0);
		//get player number
		int GetPlayerNum() { return mControllerNum; }
	};
}

