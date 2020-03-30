#pragma once

#include "D3DUtil.h"

namespace DPhoenix
{
	class Font
	{
	private:
		IDWriteFactory* mDWriteFactory;	//direct write factory pointer
		IDWriteTextFormat* mTextFormat;	//direct write text format interface
		ID2D1SolidColorBrush* mBrush;	//brush for pixel color
		HWND mhWindow;					//handle to window

		std::string mFontFamily;		//font to use
		float mSize;					//font size

	public:
		//constructor / destructor
		Font(HWND hWindow, ID2D1SolidColorBrush* brush, 
			IDWriteFactory* writeFactory, std::string fontFamily, float size);
		~Font();

		//draw font call - takes several properties to position, scale
		//color etc.
		void DrawFont(ID2D1RenderTarget* _2DRT, std::string text, float posX, float posY,
			float scaleX, float scaleY, D2D1::ColorF color);

	};
}

