#pragma once
#include "D3DUtil.h"

namespace DPhoenix
{

	//mirrors the TextureMgr class; contains a map so that
	//sounds aren't loaded more than once
	//similar to Singleton pattern; a pointer to one of these objects
	//per application can mean all objects can use sounds
	class AudioMgr
	{
	public:
		//constructor / destructor
		AudioMgr();
		~AudioMgr();

		////initialise; store pointer to D3D device
		void Init(ID3D11Device* device);

		//Create Sound new if not already stored, or present stored texture
		//Uses STL map
		//(multiple maps could be used for voice, FX, music...)
		Sound* CreateSound(std::string keyName, std::string filename);

		//return pointer to a specific sound
		Sound* GetSound(std::string keyName);

		//stop all sounds and set position to 0
		void ResetAllSounds();

	private:
		//Private constructors
		AudioMgr(const AudioMgr& rhs);
		AudioMgr& operator=(const AudioMgr& rhs);

		//STL map structure contains pointer to the sound
		std::map<std::string, Sound*> mAudioMap;
	};
}

