#include "D3DUtil.h"

namespace DPhoenix
{
	//null the global sound layer before initialisation
	SoundLayer* SoundLayer::mGlobalSLayer = NULL;

	//constructor - takes current window handle
	SoundLayer::SoundLayer(HWND hWnd)
	{
		//null pointers to directsound and primary mixer
		mDSound = NULL;
		mPrimary = NULL;

		//if this is set it is already initialised
		if (mGlobalSLayer)
		{
			debug << "SoundLayer already initialized!\n";
		}
		//set this object as the global layer
		mGlobalSLayer = this;

		HRESULT             hr;
		LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;

		// Create IDirectSound using the primary sound device
		hr = DirectSoundCreate8(NULL, &mDSound, NULL);
		if (FAILED(hr))
		{
			debug << "DirectSoundCreate failed!\n";
		}

		// Set coop level to DSSCL_PRIORITY
		//gives us more control over buffers and formats
		hr = mDSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
		if (FAILED(hr))
		{
			debug << "SetCooperativeLevel (DS) failed!\n";
		}

		// Get the primary buffer 
		DSBUFFERDESC dsbd;
		dsbd.dwBufferBytes = 0;					//0 for primary buffer (sets bounds in other buffers)
		dsbd.lpwfxFormat = NULL;				//must be NULL for primary buffer
		dsbd.dwSize = sizeof(DSBUFFERDESC);		//size of the struct in bytes
		dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;	//flag; sets this as primary buffer
		dsbd.dwReserved = 0;					//reserved bytes / not used here
		dsbd.guid3DAlgorithm = GUID_NULL;		//3D sound flags; not used

		//create the primary sound buffer based on desc
		hr = mDSound->CreateSoundBuffer(&dsbd, &pDSBPrimary, NULL);
		if (FAILED(hr))
		{
			debug << "CreateSoundBuffer (DS) failed!\n";
		}

		// Set primary buffer format to 22kHz and 16-bit output.
		WAVEFORMATEX wfx;
		ZeroMemory(&wfx, sizeof(WAVEFORMATEX));							
		wfx.wFormatTag = WAVE_FORMAT_PCM;							//standard wave format 
		wfx.nChannels = 2;											//2 channels (left / right)
		wfx.nSamplesPerSec = 22050;									//sample rate (alternative: 44100)
		wfx.wBitsPerSample = 16;									//bits per sample (alternative 32)
		wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;	//align blocks (data per sample)
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;	//average bytes per second

		//set the primary buffer format
		if (FAILED(hr = pDSBPrimary->SetFormat(&wfx)))
		{
			debug << "SetFormat (DS) failed!\n";
		}

		//release the memory
		pDSBPrimary->Release();
	}

	//destructor - releases all memory
	SoundLayer::~SoundLayer()
	{
		mPrimary->Release();
		mDSound->Release();
		mGlobalSLayer = NULL;
	}
}

