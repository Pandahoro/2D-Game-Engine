#pragma once
#include "D3DUtil.h"

namespace DPhoenix
{

	//Simple texture manager to avoid loading duplicate textures from file.  That can
	//happen, for example, if multiple sprites reference the same texture filename. 
	class TextureMgr
	{
	public:
		//constructor / destructor 
		//(could be adjusted for full adherence to Singleton design pattern)
		TextureMgr();
		~TextureMgr();

		//initialise; store pointer to D3D device
		void Init(ID3D11Device* device);

		//Create Texture new if not already stored, or present stored texture
		//Uses STL map
		ID3D11ShaderResourceView* CreateTexture(std::string filename);

	private:
		//Private constructors
		TextureMgr(const TextureMgr& rhs);
		TextureMgr& operator=(const TextureMgr& rhs);

	private:
		//pointer to D3D device
		ID3D11Device* md3dDevice;
		//STL Map structure; contains  string key and the Shader Resource View
		//A Shader Resource View (SRV) is how textures are used by a shader
		//this helps us manage loaded textures
		std::map<std::string, ID3D11ShaderResourceView*> mTextureSRV;
	};
}

