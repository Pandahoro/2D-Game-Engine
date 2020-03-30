#pragma once

#include "D3DUtil.h"

namespace PandaEngine
{

//this region is the overall effect class used to construct all effects
#pragma region Effect
	class Effect
	{
	public:
		//we need the D3D device and shader filenameto construct
		Effect(ID3D11Device* device, const std::string& filename);
		virtual ~Effect();

	private:
		Effect(const Effect& rhs);
		Effect& operator=(const Effect& rhs);

	protected:
		ID3DX11Effect* mFX;
	};
#pragma endregion

	//this class manages the sprite rendering specifically
#pragma region SpriteEffect
	class SpriteEffect : public Effect
	{
	public:
		//we need the D3D device and shader filenameto construct
		SpriteEffect(ID3D11Device* device, const std::string& filename);
		~SpriteEffect();

		//set the WorldViewProjection combined matrix
		void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
		//set the World matrix
		void SetWorld(CXMMATRIX M) { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
		//set the Diffuse map (colour-map / visible texture)
		void SetDiffuseMap(ID3D11ShaderResourceView* tex) { SpriteTex->SetResource(tex); }
		//set current column of spritesheet (for UV position)
		void SetColumn(float f) { Column->SetRawValue(&f, 0, sizeof(float)); }
		//set width of individual column (for UV mapping)
		void SetNumCols(float f) { NumCols->SetRawValue(&f, 0, sizeof(float)); }
		//set whether the sprite is flipped horizontally or not (facing left or right)
		void SetFlipValue(float f) { FlipValue->SetRawValue(&f, 0, sizeof(float)); }
		//set whether the sprite is to be interpolated between default texture and alternative color
		void SetLerpValue(float f) { LerpValue->SetRawValue(&f, 0, sizeof(float)); }
		//set an alternative colour to interpolate between the txture with
		void SetAltColorValue(const FXMVECTOR v) { AltColorValue->SetFloatVector(reinterpret_cast<const float*>(&v)); }
		//set an opacity value
		void SetOpacityValue(float f) { OpacityValue->SetRawValue(&f, 0, sizeof(float)); }

		//effect techniques that can be used
		ID3DX11EffectTechnique* SpriteTech;
		//matrix variables for the shader
		ID3DX11EffectMatrixVariable* WorldViewProj;
		ID3DX11EffectMatrixVariable* WorldViewProjTex;
		ID3DX11EffectMatrixVariable* World;
		//float values for the shader
		ID3DX11EffectVariable* Column;
		ID3DX11EffectVariable* NumCols;
		ID3DX11EffectVariable* FlipValue;
		ID3DX11EffectVariable* LerpValue;
		ID3DX11EffectVariable* OpacityValue;
		//vector variables
		ID3DX11EffectVectorVariable* AltColorValue;
		//shader resource view - this is for the sprite texture
		ID3DX11EffectShaderResourceVariable* SpriteTex;
	};
#pragma endregion

//The Effects class (with the additional s) manages
//the initialisation and use of all indivisual Effect child classes
#pragma region Effects
	class Effects
	{
	public:
		static void InitAll(ID3D11Device* device);
		static void DestroyAll();

		static SpriteEffect* SpriteFX;

	};
#pragma endregion
}

