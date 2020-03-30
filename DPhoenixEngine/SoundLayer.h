#pragma once

#include "D3DUtil.h"

namespace DPhoenix
{
	//defined elsehwhere but needs declaration here
	//as referenced in SoundLayer class declaration
	class Sound;

	class SoundLayer
	{

	private:
		LPDIRECTSOUND8 mDSound;				//DirectSound8 pointer
		LPDIRECTSOUNDBUFFER8 mPrimary;		//primary mixer
		static SoundLayer* mGlobalSLayer;	//global soundlayer pointer
		SoundLayer(HWND hWnd);				//Constructor

	public:
		virtual ~SoundLayer();				//Destructor
		static SoundLayer* GetSound()		//Get Sound (return soundlayer pointer)
		{
			return mGlobalSLayer;
		}
		//getter for directsound pointer
		LPDIRECTSOUND8 GetDSound()
		{
			return mDSound;
		}
		//create sound layer 
		static void Create(HWND hWnd)
		{
			new SoundLayer(hWnd);
		}
	};

	//return directsound pointer
	inline SoundLayer* SoundPtr()
	{
		return SoundLayer::GetSound();
	}
}

