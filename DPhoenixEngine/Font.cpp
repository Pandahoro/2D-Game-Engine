#include "D3DUtil.h"

namespace DPhoenix
{
	Font::Font(HWND hWindow, ID2D1SolidColorBrush* brush, 
		IDWriteFactory * writeFactory, std::string fontFamily, float size)
	{
		//set key pointers / values
		mDWriteFactory = writeFactory;
		mFontFamily = fontFamily;
		mSize = size;
		mBrush = brush;
		mhWindow = hWindow;
		
		//need to convert to wstring before const WCHAR* for this next method
		std::wstring fontString(mFontFamily.begin(), mFontFamily.end());

		HRESULT hr = mDWriteFactory->CreateTextFormat(
			fontString.c_str(),					// Font family name.
			NULL,								// Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,			//font weight (regular / bold etc.)
			DWRITE_FONT_STYLE_NORMAL,			//font style (e.g. italic)
			DWRITE_FONT_STRETCH_NORMAL,			//font stretching
			size,								//font size
			L"en-us",							//language
			&mTextFormat						//output pointer o textformat
		);

		//LEADING means left aligned / as does NEAR
		if (SUCCEEDED(hr))
		{
			hr = mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		}

		if (SUCCEEDED(hr))
		{
			hr = mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		}

	}
	
	//destructor
	Font::~Font()
	{
		ReleaseCOM(mTextFormat);
	}

	//draws font to screen
	void Font::DrawFont(ID2D1RenderTarget* _2DRT, std::string text, float posX, float posY,
		float scaleX, float scaleY, D2D1::ColorF color)
	{

		//need to convert to wstring before const WCHAR* for this next method
		std::wstring textString(text.begin(), text.end());
		
		//length of text string, used to calculate draw-to rectangle
		UINT32 cTextLength_ = textString.length();

		//get the window size
		RECT rc;
		GetClientRect(mhWindow, &rc);

		//layout of rectangle area to draw text to
		//based on position relative to window edges 
		//and length of text string
		D2D1_RECT_F layoutRect = D2D1::RectF(
			static_cast<FLOAT>(rc.left + posX),
			static_cast<FLOAT>(rc.top + posY),
			static_cast<FLOAT>(rc.left + posX * mSize * cTextLength_),
			static_cast<FLOAT>(rc.top + posY * mSize)
		);

		//set new brush colour from render target
		_2DRT->CreateSolidColorBrush(
			D2D1::ColorF(color),
			&mBrush
		);

		//start drawing text
		_2DRT->BeginDraw();

		//set transform properties to scale the text
		_2DRT->SetTransform(D2D1::Matrix3x2F::Scale(
			D2D1::Size(scaleX, scaleY),
			D2D1::Point2F(0.0f, 0.0f)));


		// Call the DrawText method of the 2D render target.
		_2DRT->DrawText(
			textString.c_str(),			// The string to render.
			cTextLength_,				// The string's length.
			mTextFormat,				// The text format.
			layoutRect,					// The region of the window where the text will be rendered.
			mBrush						// The brush used to draw the text.
		);

		//finish drawing text
		_2DRT->EndDraw();

	}
}

