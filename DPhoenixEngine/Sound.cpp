#include "D3DUtil.h"

namespace DPhoenix
{
	//map existing sound son the layer
	std::map< WaveSoundRead*, int > Sound::mWaveMap;

	Sound::Sound(char* filename)
	{
		//null wave reader and buffer pointers
		mWaveSoundRead = NULL;
		mBuffer = NULL;

		// Create a new wave file class
		mWaveSoundRead = new WaveSoundRead();
		mWaveMap[mWaveSoundRead] = 1;

		// Load the wave file
		if (FAILED(mWaveSoundRead->Open(filename)))
		{
			debug << "couldn't open sound file! " << filename << "\n";
		}

		//initialise the sound buffer and fill it with the wave data
		Init();
		Fill();
	}

	Sound::Sound(Sound& in)
	{
		//here we take in an existing sound 
		//pointer to existing data
		mWaveSoundRead = in.mWaveSoundRead;
		//add to map counter of number of sounds
		mWaveMap[mWaveSoundRead]++;
		//initialise the sound buffer and fill it with the wave data
		Init();
		Fill();
	}

	Sound& Sound::operator=(const Sound &in)
	{
		//Destroy the old object
		int count = --mWaveMap[mWaveSoundRead];
		if (!count)
		{
			delete mWaveSoundRead;
		}
		mBuffer->Release();

		//Clone the incoming one
		mWaveSoundRead = in.mWaveSoundRead;
		mWaveMap[mWaveSoundRead]++;
		Init();
		Fill();
		return *this;

	}

	Sound::~Sound()
	{
		//release memory for active pointers
		int count = mWaveMap[mWaveSoundRead];
		if (count == 1)
		{
			delete mWaveSoundRead;
		}
		else
		{
			mWaveMap[mWaveSoundRead] = count - 1;
		}

		mBuffer->Release();
	}

	void Sound::Init()
	{
		/**
		* Set up the DirectSound surface. The size of the sound file
		* and the format of the data can be retrieved from the wave
		* sound object. Besides that, we only set the STATIC flag,
		* so that the driver isn't restricted in setting up the
		* buffer.
		*/
		DSBUFFERDESC dsbd;
		dsbd.dwFlags = DSBCAPS_CTRLVOLUME;					//this lets us control the volume
		dsbd.dwBufferBytes = mWaveSoundRead->mCkIn.cksize;	//bytes for buffer from wave file
		dsbd.lpwfxFormat = mWaveSoundRead->mWfx;			//get the wave format
		dsbd.guid3DAlgorithm = GUID_NULL;					//null for 3D sound
		dsbd.dwSize = sizeof(DSBUFFERDESC);					//size of the desc
		dsbd.dwReserved = 0;								//no reserved bytes

		HRESULT hr;

		//Temporary pointer to old DirectSound interface
		LPDIRECTSOUNDBUFFER pTempBuffer = 0;

		//Create the sound buffer
		hr = DPhoenix::SoundPtr()->GetDSound()->CreateSoundBuffer(&dsbd, &pTempBuffer, NULL);
		if (FAILED(hr))
		{
			debug << "CreateSoundBuffer failed \n";
		}

		//Upgrade the sound buffer to version 8
		pTempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&mBuffer);
		if (FAILED(hr))
		{
			debug << "SoundBuffer query to 8 failed \n";
		}

		//Release the temporary old buffer
		pTempBuffer->Release();

		/**
		*Remember how big the buffer is
		*/

		mBufferSize = dsbd.dwBufferBytes;
	}

	void Sound::Restore()
	{
		//check we have an existing buffer
		HRESULT hr;
		if (NULL == mBuffer)
		{
			return;
		}

		//check buffer okay
		DWORD dwStatus;
		if (FAILED(hr = mBuffer->GetStatus(&dwStatus)))
		{
			debug << "SoundBuffer query to 8 failed! \n";
		}

		if (dwStatus & DSBSTATUS_BUFFERLOST)
		{
			/**
			* Chances are, we got here because the app /just/
			* started, and DirectSound hasn't given us any
			* control yet. Just spin until we can restore
			* the buffer
			*/
			do
			{
				hr = mBuffer->Restore();
				if (hr == DSERR_BUFFERLOST)
					Sleep(10);
			} while (hr = mBuffer->Restore());

			/**
			* The buffer was restored. Fill 'er up.
			*/
			Fill();
		}
	}

	void Sound::Fill()
	{
		HRESULT hr;
		unsigned char* pbWavData;	//Pointer to actual wav data
		unsigned int cbWavSize;		//size of data
		void* pbData = NULL;		//pointer to buffer data
		void* pbData2 = NULL;
		unsigned long dwLength;		//wave data length
		unsigned long dwLength2;

		/**
		*How big the wav file is
		*/
		unsigned int nWaveFileSize = mWaveSoundRead->mCkIn.cksize;

		/**
		* Allocate enough data to hold the wav file data
		*/
		pbWavData = new unsigned char[nWaveFileSize];
		if (NULL == pbWavData)
		{
			delete[] pbWavData;
			debug << "Out of memory! \n";
		}

		//read the wave data
		hr = mWaveSoundRead->Read(
			nWaveFileSize,
			pbWavData,
			&cbWavSize);
		if (FAILED(hr))
		{
			delete[] pbWavData;
			debug << "mWaveSoundRead->Read failed\n";
		}

		/**
		* Reset the file to the beginning
		*/
		mWaveSoundRead->Reset();

		/**
		* Lock the buffer so we can copy the data over
		*/
		hr = mBuffer->Lock(
			0, mBufferSize, &pbData, &dwLength,
			&pbData2, &dwLength2, 0L);
		if (FAILED(hr))
		{
			delete[] pbWavData;
			debug << "mBuffer->Lock failed\n";
		}

		/**
		* Copy said data over, unlocking afterwards
		*/
		memcpy(pbData, pbWavData, mBufferSize);
		mBuffer->Unlock(pbData, mBufferSize, NULL, 0);

		/**
		* We're done with the wav data memory.
		*/
		delete[] pbWavData;
	}

	//check if the sound is currently playing
	bool Sound::IsPlaying()
	{
		//get status from buffer
		DWORD dwStatus = 0;
		mBuffer->GetStatus(&dwStatus);

		//check status of buffer
		if (dwStatus & DSBSTATUS_PLAYING)
			return true;
		else
			return false;
	}

	//play the sound - boolean param is for looping sounds
	void Sound::Play(bool bLoop)
	{
		HRESULT hr;
		if (NULL == mBuffer)
			return;

		// Restore the buffers if they are lost
		Restore();

		// Play buffer
		DWORD dwLooped = bLoop ? DSBPLAY_LOOPING : 0L;
		if (FAILED(hr = mBuffer->Play(0, 0, dwLooped)))
		{
			debug << "mBuffer->Play failed\n";
		}
	}

	//set position in file (milis)
	void Sound::SetPosition(int pos) {
		mBuffer->SetCurrentPosition(pos);
	}

	//stop the sound (effectively pausing)
	void Sound::Stop()
	{
		HRESULT hr;
		if (NULL == mBuffer)
			return;

		// Restore the buffers if they are lost
		Restore();
		if (FAILED(hr = mBuffer->Stop()))
		{
			debug << "mBuffer->Stop failed\n";
		}
	}
}

