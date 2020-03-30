//#include "Engine.h"
//
////initial class definition inheriting fron Engine
//class InitDirect3DApp : public DPhoenix::Engine
//{
//public:
//	InitDirect3DApp(HINSTANCE hInstance);
//	~InitDirect3DApp();
//
//	bool Init(bool fullScreen);
//	void OnResize();
//	void UpdateScene(float dt);
//	void DrawScene();
//};
//
////our epplication entry point - WinMain
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
//	PSTR cmdLine, int showCmd)
//{
//	// Enable run-time memory check for debug builds.
//	#if defined(DEBUG) | defined(_DEBUG)
//		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	#endif
//
//	//create our demo app
//	InitDirect3DApp theApp(hInstance);
//
//	//try initialisation
//	if (!theApp.Init(false))
//		return 0;
//
//	//now run the application
//	return theApp.Run();
//}
//
////constructor - don't really need to add much here
//InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
//	: Engine(hInstance)
//{
//}
//
////destructor (release any memory as necessary)
//InitDirect3DApp::~InitDirect3DApp()
//{
//}
//
////any additional initialisation (we generally initiaise game assets here)
//bool InitDirect3DApp::Init(bool fullScreen)
//{
//	if (!Engine::Init(fullScreen))
//		return false;
//
//	return true;
//}
//
//
////what to do on resize - mainly we'lll just call the parent / engine's method
//void InitDirect3DApp::OnResize()
//{
//	Engine::OnResize();
//}
//
////update scene - here is where our update logic for the game loop goes
//void InitDirect3DApp::UpdateScene(float dt)
//{
//
//}
//
////draw the scene - here's where our rendering calls go
//void InitDirect3DApp::DrawScene()
//{
//	//do we have the context and swapchain?
//	assert(md3dImmediateContext);
//	assert(mSwapChain);
//
//	//clear the screen with the defined colour
//	float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
//	//set the render target view and the depth stencil
//	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, clearColor);
//	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//
//	//present the backbuffer / swapchain to the screen
//	HR(mSwapChain->Present(0, 0));
//}