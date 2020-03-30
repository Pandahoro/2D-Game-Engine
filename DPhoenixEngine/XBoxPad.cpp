#include "D3DUtil.h"

namespace DPhoenix
{
	XboxPad::XboxPad(int playerNumber)
	{
		//set the controller number
		mControllerNum = playerNumber - 1;
	}

	XINPUT_STATE XboxPad::GetState()
	{
		//Zeroise the state
		ZeroMemory(&mControllerState, sizeof(XINPUT_STATE));

		// Get the state
		XInputGetState(mControllerNum, &mControllerState);

		return mControllerState;
	}

	bool XboxPad::IsConnected()
	{
		// Zeroise the state
		ZeroMemory(&mControllerState, sizeof(XINPUT_STATE));
		//get the state
		DWORD Result = XInputGetState(mControllerNum, &mControllerState);

		if (Result == ERROR_SUCCESS)
		{
			return true;
		}
		else {
			return false;
		}
	}

	void XboxPad::Vibrate(int leftVal, int rightVal)
	{
		// Create a Vibraton State
		XINPUT_VIBRATION Vibration;

		// Zeroise the Vibration
		ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

		// Set the Vibration Values
		Vibration.wLeftMotorSpeed = leftVal;
		Vibration.wRightMotorSpeed = rightVal;

		// Vibrate the controller
		XInputSetState(mControllerNum, &Vibration);
	}
}