#pragma once

#include "D3DUtil.h"

//our engine will be called DPhoenix
//Maybe DP for short?  Anyway, sounds cool
namespace DPhoenix
{
	class Engine
	{
		//we use protected because individual 'apps' / games will
		//inherit from the engine class to then implement properties / methods
		//to be game specific
	protected:

		//main window / render flags / properties ----------------------------------------------

		HINSTANCE	mhAppInst;			//app instance handle
		HWND		mhMainWnd;			//window handle (main window)
		bool		mAppPaused;			//is app paused - flag
		bool		mMinimized;			//is app minized - flag
		bool		mMaximized;			//is app maximised - flag
		bool		mResizing;			//is app resizing - flag
		UINT		m4xMsaaQuality;		//quality value for 4xMSAA (if we are using it)
		bool		mFullScreen;		//flag for full screen mode

		GameTimer mTimer;				//game timer class instance
		
		//main rendering interfaces and properties ----------------------------------------------
		
		ID3D11Device* md3dDevice;					//a reference to the D3D device in memory
		ID3D11DeviceContext* md3dImmediateContext;	//the device context; what we are rendering on
		IDXGISwapChain* mSwapChain;					//swap chain; what we are rendering with
		ID3D11Texture2D* mDepthStencilBuffer;		//depth stencil buffer; (we use the depth test)
													//used to position objects relative to each other
													//nearest on top, like life
		ID3D11RenderTargetView* mRenderTargetView;	//render target view; what we are rendering to 
													//to present to the screen (like a backbuffer)
		ID3D11DepthStencilView* mDepthStencilView;	//a view of the depth / stencil texture to be sent 
													//to the GPU / shader pipeline
		D3D11_VIEWPORT mScreenViewport;				//where we are 'Present'-ing the render target to
													//the screen (can be other viewports)

													// Derived class should set these in derived 
													//constructor to customize starting values.
		std::string mMainWndCaption;				//main window caption
		D3D_DRIVER_TYPE md3dDriverType;				//D3D driver type
		int mClientWidth;							//client width -  screen
		int mClientHeight;							//client height -- screen
		bool mEnable4xMsaa;							//flag - are we using 4xMSAA quality?
													//4x multi-sampled anti-aliaising
													//longer to render, nicer to look at
													//less 'jaggies'

													//members for DirectWrite (fonts)
		ID2D1Factory* mD2DFactory;					//Direct2D factory object (creates things)
		IDWriteFactory* mDWriteFactory;				//direct write factory pointer

		ID2D1RenderTarget* m2D1RT;					//Render target (our window)
		ID2D1SolidColorBrush* mBlackBrush;			//black brush for pixel color

		//main methods for initialisation ---------------------------------------------------

		bool InitMainWindow();						//initialise main window
		bool InitDirect3D();						//initialise D3D device / swapchain etc.
		void CalculateFrameStats();					//calculate frame stats (logic / rendering)

		//Input and event handling -----------------------------------------------------------
		//Input pointer and input methods
		Input* mInput;
		void UpdateKeyboard();
		void UpdateMouse();

		XboxPad *mXboxInput;
		void UpdateXbox();

		//fire event to be handled
		void RaiseEvent(IEvent* e);

	public:
		Engine(HINSTANCE hInstance);
		virtual ~Engine();

		//getters for key values
		HINSTANCE AppInst()const;
		HWND      MainWnd()const;
		float     AspectRatio()const;

		//contains the main game loop
		int Run();

		// Framework methods.  Derived client class overrides these methods to 
		// implement specific application requirements.

		//initialise engine
		virtual bool Init(bool fullScreen);
		//called when window resizes - creates new backbuffer / render target
		virtual void OnResize();
		//updates the engine as part of the game loop
		virtual void UpdateScene(float dt) = 0;
		//renders as part of the game loop
		virtual void DrawScene() = 0;
		//windows message loop callback function
		//so windows messages can be handled in-engine
		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		//handles events (e.g. input)
		virtual void HandleEvents(IEvent* e) = 0;
	};
}