#pragma once

#include "D3DUtil.h"

class WaveSoundRead
{
public:
	WAVEFORMATEX* mWfx;		// Pointer to WAVEFORMATEX structure
	HMMIO mHmmioIn;			// MM I/O handle for the WAVE
	MMCKINFO mCkIn;			// Multimedia RIFF chunk
	MMCKINFO mCkInRiff;		// Use in opening a WAVE file
public:
	WaveSoundRead();
	~WaveSoundRead();

	//main loading oprations for a wave file; implemented in provided
	//source code file (standard Microsoft code)
	HRESULT Open(CHAR* strFilename);
	HRESULT Reset();
	HRESULT Read(UINT nSizeToRead, BYTE* pbData, UINT* pnSizeRead);
	HRESULT Close();
};

namespace DPhoenix
{
	class Sound {
	private:
		WaveSoundRead* mWaveSoundRead;			//pointer to our wave sound reader
		LPDIRECTSOUNDBUFFER8 mBuffer;			//direct sound buffer pointer
		int mBufferSize;						//size of buffer
		/**
		FROM Advanced 3D Game Programming using DirectX 9.0
		copyright (c) 2002 by Peter A Walsh and Adrian Perez:
		* Multiple sounds that use the same
		* file shouldn't reread it, they should
		* share the CWSR object. This map
		* implements rudimentary reference counting.
		* I would have just changed WaveSoundRead,
		* but I wanted to keep it unchanged from the
		* samples.
		*/
		static std::map< WaveSoundRead*, int > mWaveMap;
		//initialise the sound
		void Init();
	public:
		//constructors - loads sound file
		Sound(char* filename);
		Sound(Sound& in);
		Sound& operator=(const Sound &in);
		//destructor
		virtual ~Sound();

		//main audio controls (you could add more...)
		void Restore();
		void Fill();
		void Play(bool bLoop = false);
		bool IsPlaying();
		void Stop();
		void SetPosition(int pos);
	};
}

