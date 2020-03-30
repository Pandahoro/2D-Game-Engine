#include "Effect.h"

namespace DPhoenix
{

//Effect class sets up ability to compile shaders and then use them
//within the code for rendering
#pragma region Effect
	Effect::Effect(ID3D11Device* device, const std::string& filename)
		: mFX(0)
	{
		DWORD shaderFlags = 0;
		//yeah it's D3D10, they just didn't update this part
		//in DX11
		#if defined(DEBUG) || defined(_DEBUG)
				shaderFlags |= D3D10_SHADER_DEBUG;
				shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
		#endif

		//THE BLOB!!!! (data for shader compilation)
		ID3D10Blob* compiledShader = 0;
		ID3D10Blob* compilationMsgs = 0;

		//load this particular shader (generic function should be easy)
		HRESULT hr = D3DX11CompileFromFile(
						filename.c_str(),		//filename
						0,						//array of macro definitions
						0,						//any included files
						0,						//any entry point for shader file
						"fx_5_0",				//FX / HLSL version
						shaderFlags,			//any shader flags (see above)
						0,						//additional effect compile flags
						0,						//thread pump
						&compiledShader,		//output pointer to compiled shader	
						&compilationMsgs,		//output pointer to any compiation messages
						0);						//output pointer to return value

		// compilationMsgs can store errors or warnings.
		//hence message if there are any
		if (compilationMsgs != 0)
		{
			MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationMsgs);
		}

		// Even if there are no compilationMsgs, check to make sure there
		// were no other errors.
		if (FAILED(hr))
		{
			DXTrace(__FILE__, (DWORD)__LINE__, hr,
				"D3DX11CompileFromFile", true);
		}

		//mFX wil store the compiled effect
		HR(D3DX11CreateEffectFromMemory(
			compiledShader->GetBufferPointer(),
			compiledShader->GetBufferSize(),
			0, device, &mFX));

		// Done with compiled shader.
		ReleaseCOM(compiledShader);
	}

	Effect::~Effect()
	{
		ReleaseCOM(mFX);
	}
#pragma endregion

//this defines the SpriteEffect class
#pragma region SpriteEffect
	//constructor - get device and filename (call parent constructor first)
	SpriteEffect::SpriteEffect(ID3D11Device* device, const std::string& filename)
		: Effect(device, filename)
	{
		//set the SpriteTech to the shader technique
		SpriteTech = mFX->GetTechniqueByName("Sprite");

		//link Effect variables to shader variables (for get / set)
		//Matrices
		WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
		World = mFX->GetVariableByName("gWorld")->AsMatrix();
		//asprite texture (shader resource view)
		SpriteTex = mFX->GetVariableByName("gSpriteTex")->AsShaderResource();
		//float values
		Column = mFX->GetVariableByName("gColumn")->AsScalar();
		NumCols = mFX->GetVariableByName("gNumCols")->AsScalar();
		FlipValue = mFX->GetVariableByName("gFlipValue")->AsScalar();
		LerpValue = mFX->GetVariableByName("gLerpValue")->AsScalar();
		OpacityValue = mFX->GetVariableByName("gOpacityValue")->AsScalar();
		//vector (color) value
		AltColorValue = mFX->GetVariableByName("gAltColor")->AsVector();
	}

	SpriteEffect::~SpriteEffect()
	{
	}
#pragma endregion

#pragma region Effects

	SpriteEffect*       Effects::SpriteFX = 0;

	void Effects::InitAll(ID3D11Device* device)
	{
		SpriteFX = new SpriteEffect(device, "FX/Sprite.fx");
	}

	void Effects::DestroyAll()
	{
		SafeDelete(SpriteFX);
	}
#pragma endregion

}

