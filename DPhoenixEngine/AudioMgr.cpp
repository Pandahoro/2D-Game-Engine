#include "AudioMgr.h"

//blank constructor
PandaEngine::AudioMgr::AudioMgr()
{

}

PandaEngine::AudioMgr::~AudioMgr()
{
	//(second in map structures refers to the second data item)
	//(usually the data rather than the key, in this case)
	//(a Sound pointer)

	//iterate through whole map and release each Sound
	for (auto it = mAudioMap.begin(); it != mAudioMap.end(); ++it)
	{
		it->second->~Sound();
	}

	//clear the STL structure
	mAudioMap.clear();
}

void PandaEngine::AudioMgr::Init(ID3D11Device * device)
{

}

//Create texture or supply existing - takes in key / filename
PandaEngine::Sound* PandaEngine::AudioMgr::CreateSound(std::string keyName,
	std::string filename)
{
	Sound* sound = 0;

	// Does it already exist? if so supply it
	if (mAudioMap.find(keyName) != mAudioMap.end())
	{
		sound = mAudioMap[keyName];
	}
	else
	{
		//otherwise we need to load it - copy string to right format
		char* fileNameConvert = new char[filename.size() + 1];
		strcpy(fileNameConvert, filename.c_str());

		//load the sound
		sound = new PandaEngine::Sound(fileNameConvert);

		//store in map
		mAudioMap[keyName] = sound;

		//free up memory
		delete[] fileNameConvert;
	}
	//return the sound pointer
	return sound;
}

//retrieves a sound pointer by key if it already exists
PandaEngine::Sound * PandaEngine::AudioMgr::GetSound(std::string keyName)
{
	// Does it already exist?
	if (mAudioMap.find(keyName) != mAudioMap.end())
	{
		return mAudioMap[keyName];
	}
	else
	{
		return nullptr;
	}
}

//stop all sounds and set position to 0
void PandaEngine::AudioMgr::ResetAllSounds()
{
	//(second in map structures refers to the second data item)
	//(usually the data rather than the key, in this case)
	//(a Sound pointer)

	//iterate through the map
	for (std::map<std::string, Sound*>::iterator it = mAudioMap.begin(); 
			it != mAudioMap.end(); ++it)
	{
		//top and set position to 0 (beginning)
		it->second->Stop();
		it->second->SetPosition(0);
	}
}

