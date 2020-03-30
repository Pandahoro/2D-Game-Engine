#include "TextureMgr.h"

namespace DPhoenix 
{

	//constructor - zero device pointer until init
	TextureMgr::TextureMgr() : md3dDevice(0)
	{
	}

	//destrctor - release all memory
	TextureMgr::~TextureMgr()
	{
		//iterate through whole map and release wach Shader Resource View
		for (auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
		{
			ReleaseCOM(it->second);
		}

		//clear the STL structure
		mTextureSRV.clear();
	}

	//initialise; keep pointer to D3D device
	void TextureMgr::Init(ID3D11Device* device)
	{
		md3dDevice = device;
	}

	//Create texture or supply existing - takes in filename
	ID3D11ShaderResourceView* TextureMgr::CreateTexture(std::string filename)
	{
		//zero SRV interface before use
		ID3D11ShaderResourceView* srv = 0;

		// Does it already exist?
		if (mTextureSRV.find(filename) != mTextureSRV.end())
		{
			//f so present the existing
			srv = mTextureSRV[filename];
		}
		else
		{
			//if not, create new from file
			HR(D3DX11CreateShaderResourceViewFromFile(
				md3dDevice,				//Pointer to the Direct3D device
				filename.c_str(),		//filename of texture to load
				0,						//any load information flags
				0,						//thread pump
				&srv,					//output pointer to shader resource view
				0));					//output pointer to return value

			mTextureSRV[filename] = srv;	//add to map
		}

		//return the Shader Resource View
		return srv;
	}
}

