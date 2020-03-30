#pragma once

//check if in debug mode (not release)
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>					//this can help detect memory leaks
#endif


#include <cwchar>						//helper for 'wide' character sets
#include <d3d11.h>						//main Direct3D 11 header
#include <d3dx11.h>						//DX11 extras header
#include <xnamath.h>					//XNA Math header (VERY useful)
#include <dxerr.h>						//DX ERR header (help with DX errors)
#include <cassert>						//assert header; can check values as in-built tests
#include <ctime>						//time; used when constructing the timer
#include <algorithm>					//standard algorithms header
#include <string>						//string data type
#include <sstream>						//string stream - good for concatenations etc.
#include <fstream>						//file stream for when reading in files
#include <vector>						//STL vector data structure
#include <map>							//STL map data structure
#include "d3dx11Effect.h"				//DX11 Effect headder for HLSL compatibility

#include <dinput.h>						//DirectInput
#include <XInput.h>						//XBox Pads
#include <mmsystem.h>					//for audio
#include <mmreg.h>						//for audio
#include <dsound.h>						//for audio

//direct write for font rendering
#include <d2d1.h>
#include <dwrite.h>


//DirectSound may well have been deprecated for XAct2
//However it provides us an easy way to use sounds
//Hence I'm using it and legacy definitions to compile

#pragma comment(lib, "d3d11.lib")						//Direct3D11
#pragma comment(lib, "d3dx11d.lib")						//D3D11 extras
#pragma comment(lib, "dsound.lib")						//DirectSound
#pragma comment(lib, "dinput8.lib")						//DirectInput8
#pragma comment(lib, "xinput.lib")						//X-Input (pads)
#pragma comment(lib, "winmm.lib")						//Windows MultiMedia
#pragma comment(lib, "D3DCompiler.lib")					//shader compilation
#pragma comment(lib, "dxerr.lib")						//DX Err checking
#pragma comment(lib, "dxgi.lib")						//DX graphics infrastructure
#pragma comment(lib, "dxguid.lib")						//Global ID
#pragma comment(lib, "legacy_stdio_definitions.lib")	//ensure all libraries are compatible

//direct write for font rendering
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

//http://www.codeguru.com/cpp/misc/misc/graphics/article.php/c16139/Introduction-to-DirectWrite.htm
//https://msdn.microsoft.com/en-us/library/windows/desktop/ff934857.aspx
//https://msdn.microsoft.com/en-us/library/windows/desktop/dd368152(v=vs.85).aspx


//this lets us use HLSL shaders
//(rather than compile separate vertex / pixel shaders)
//there is a debug and release version, hence the pre-processor if
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib,"Effects11d.lib") 
#else
#pragma comment(lib,"Effects11.lib") 
#endif

//---------------------------------------------------------------------------------------
// Simple d3d error checker - wrapper for DXERR Trace command
//---------------------------------------------------------------------------------------

//Essentially if we are in debug mode we will check to see if a DirectX
//method is working (which will take the place of x)
//if it has failed we'll trace the error and output it
//DXTraceW means we are using the version for the multi-byte character set

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)													\
			{																\
				HRESULT hr = (x);											\
				if(FAILED(hr))												\
				{															\
					DXTraceW(__FILE__, (DWORD)__LINE__, hr, L#x, TRUE);		\
				}															\
			}
#endif
//if in release mode, just run the method
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 


//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }

//---------------------------------------------------------------------------------------
// Headers for Engine Files
//---------------------------------------------------------------------------------------

#include "Logfile.h"
#include "GameTimer.h"
#include "Effect.h"
#include "Vertex.h"
#include "TextureMgr.h"
#include "Input.h"
#include "XBoxPad.h"
#include "Event.h"
#include "SoundLayer.h"
#include "Sound.h"
#include "AudioMgr.h"
#include "Sprite.h"
#include "Font.h"

