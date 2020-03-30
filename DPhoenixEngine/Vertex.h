#pragma once
#include "D3DUtil.h"

//put all vertex structs in thir own namespace for convenience
namespace Vertex
{
	//3 floats for x,y,z position
	//2 floats for u,v texture co-ords
	struct SpritePosTex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
	};

}

namespace DPhoenix
{
	//InputLayoutDesc class necessary to define 'input layout'
	//for the shader (e.g. how the data should be treated)
	class InputLayoutDesc
	{
	public:
		// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
		static const D3D11_INPUT_ELEMENT_DESC SpritePosTex[2];

	};

	//class to manage all input layouts so they are all available at compile time
	class InputLayouts
	{
	public:
		static void InitAll(ID3D11Device* device);
		static void DestroyAll();

		static ID3D11InputLayout* SpritePosTex;

	};
}
