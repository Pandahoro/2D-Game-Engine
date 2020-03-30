//#include "Engine.h"
//
////initial class definition inheriting fron Engine
//class FontDemo : public DPhoenix::Engine
//{
//private:
//	//depth stencil (to handle multiple overlayed sprites)
//	ID3D11DepthStencilState* mDepthStencilState;
//	ID3D11DepthStencilState* mDepthDisabledStencilState;
//	D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;
//	D3D11_DEPTH_STENCIL_DESC mDepthDisabledStencilDesc;
//
//	//vertex buffer and index buffer for the sprite
//	ID3D11Buffer* mSpriteVB;
//	ID3D11Buffer* mSpriteIB;
//
//	//texture manager
//	DPhoenix::TextureMgr mTexMgr;
//
//	//audio manager
//	DPhoenix::AudioMgr mAudioMgr;
//
//	//Sprite pointer - character
//	DPhoenix::Sprite* mCharacterSprite;
//
//	//font pointer - debug text
//	DPhoenix::Font* mDebugFont;
//
//	//screen scale (for window stretching)
//	XMFLOAT2 mScreenScale;
//
//	//sprite position
//	XMFLOAT3 mPosition;
//	XMFLOAT3 mVelocity;
//
//	//world, view and projection matrices
//	XMFLOAT4X4 mWorld;
//	XMFLOAT4X4 mView;
//	XMFLOAT4X4 mProj;
//
//	//offset and counter vars for the vertices / indices
//	int mSpriteVertexOffset;
//	UINT mSpriteIndexOffset;
//	UINT mSpriteIndexCount;
//public:
//	FontDemo(HINSTANCE hInstance);
//	~FontDemo();
//
//	bool Init(bool fullScreen);
//	void OnResize();
//	void UpdateScene(float dt);
//	void DrawScene();
//
//	//new function; this builds our quad for the sprite
//	void BuildGeometryBuffers();
//
//	//new method / implements event handling
//	void FontDemo::HandleEvents(DPhoenix::IEvent* e);
//
//	//new method / initialises sounds to be used in the app
//	void InitAudio();
//
//	//new method / specifically handles individual sprite rendering
//	void RenderSprite(DPhoenix::Sprite* sprite,
//		ID3DX11EffectTechnique* tech, bool isHUD = false);
//};
//
////our epplication entry point - WinMain
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
//	PSTR cmdLine, int showCmd)
//{
//	// Enable run-time memory check for debug builds.
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//	FontDemo theApp(hInstance);
//
//	if (!theApp.Init(false))
//		return 0;
//
//	return theApp.Run();
//}
//
////constructor - don't really need to add much here
//FontDemo::FontDemo(HINSTANCE hInstance)
//	: Engine(hInstance)
//{
//	mMainWndCaption = "Font Demo";
//
//}
//
////destructor (release any memory as necessary)
//FontDemo::~FontDemo()
//{
//	DPhoenix::Effects::DestroyAll();
//	DPhoenix::InputLayouts::DestroyAll();
//}
//
////any additional initialisation (we generally initiaise game assets here)
//bool FontDemo::Init(bool fullScreen)
//{
//	if (!Engine::Init(fullScreen))
//		return false;
//
//	//initialise the texture manager
//	mTexMgr.Init(md3dDevice);
//
//	// Must init Effects first since InputLayouts depend on shader signatures.
//	DPhoenix::Effects::InitAll(md3dDevice);
//	DPhoenix::InputLayouts::InitAll(md3dDevice);
//
//	//must init soundlayer to then buffer audio
//	DPhoenix::SoundLayer::Create(mhMainWnd);
//	InitAudio();
//
//	//build render / blend modes etc.
//	BuildGeometryBuffers();
//
//	//create new animated sprite
//	mCharacterSprite = new DPhoenix::Sprite();
//	mCharacterSprite->Load("Textures\\DeanyP\\Run.png", &mTexMgr, 75.0f, 100.0f, md3dDevice);
//	mCharacterSprite->SetCurrentFrame(0);	mCharacterSprite->mAnimationColumns = 12;
//	mCharacterSprite->mAnimationDirection = 16;	mCharacterSprite->SetAnimationRange(0, 11);
//	mCharacterSprite->SetScale(1.0f);	mCharacterSprite->mFlipValue = 1.0f;
//
//	//set initial position and velocity
//	mPosition.x = 5.0f; mPosition.y = 5.0f; mPosition.z = 0.0f;
//	mCharacterSprite->mPosition = mPosition;
//
//	mVelocity.x = 0.0f; mVelocity.y = 0.0f; mVelocity.z = 0.0f;
//
//	//debug font
//	//(Arial size 20 sending window, initial brush, writefactory)
//	mDebugFont = new DPhoenix::Font(mhMainWnd, mBlackBrush, mDWriteFactory, "Arial", 20.0f);
//	
//	return true;
//}
//
////what to do on resize - mainly we'lll just call the parent / engine's method
//void FontDemo::OnResize()
//{
//	Engine::OnResize();
//}
//
////update scene - here is where our update logic for the game loop goes
//void FontDemo::UpdateScene(float dt)
//{
//	//calculate scaling based on window dimensions
//	mScreenScale.x = (float)mClientWidth / 1440;
//	mScreenScale.y = (float)mClientHeight / 900;
//
//	//move position by velocity
//	mCharacterSprite->mVelocity = mVelocity;
//	//update sprite passing delta time
//	mCharacterSprite->Update(dt);
//
//	//reset velocity after each update as determined by input
//	mVelocity.x = 0.0f; mVelocity.y = 0.0f;
//}
//
//void FontDemo::BuildGeometryBuffers()
//{
//#pragma region GeometryCreation
//
//
//#pragma endregion GeometryCreation
//
//#pragma region BlendStates
//	//CREATE BLEND STATE ==========================================
//	//this is so transparency can be layered for our sprites
//
//	//create blend description struct and set properties
//	D3D11_BLEND_DESC blendDesc;
//	ZeroMemory(&blendDesc, sizeof(blendDesc));
//	blendDesc.RenderTarget[0].BlendEnable = TRUE;						//enable alpha blend
//	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//blend operation (add colour)
//	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//source blend - alpha 
//	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	//dest blend - inverse source alpha
//	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;		//blend operation for alpha
//	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;			//blend based fully on source
//	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;		//not blended based on destination
//																		//this can affect the blend / transparency with different hex values
//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//	//blend state (allow transparency)
//	ID3D11BlendState* alphaBlendState_;
//	//create the blend state with the properties set
//	//OM stands for output merger - final stage of shader pipeline
//	md3dDevice->CreateBlendState(&blendDesc, &alphaBlendState_);
//	md3dImmediateContext->OMSetBlendState(alphaBlendState_, 0, 0xffffffff);
//#pragma endregion BlendStates
//
//#pragma region DepthStencilStates
//
//	//DEPTH STENCIL STATES ==================================================
//	//disabled depth stenciling will enable for 2D sprites to be layered on top of each other
//	//we will use the standard depth stencilling for 3D rendering
//
//	// Initialize the description of the stencil state.
//	ZeroMemory(&mDepthStencilDesc, sizeof(mDepthStencilDesc));
//
//	// Set up the description of the stencil state.
//	//enable depth checking
//	mDepthStencilDesc.DepthEnable = true;
//	//use full buffer for depth checks
//	mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	//compare depth data
//	//If the source data is less than the destination data, the comparison passes.
//	mDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//
//
//	//enable stencil testing
//	mDepthStencilDesc.StencilEnable = true;
//	//use all buffer for reading
//	mDepthStencilDesc.StencilReadMask = 0xFF;
//	//use all buffer for writing
//	mDepthStencilDesc.StencilWriteMask = 0xFF;
//
//	// Stencil operations if pixel is front-facing.
//	//keep if stencil test fails
//	mDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	//increment if stencil and depth fails
//	mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	//keep if stencil test passes
//	mDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	//always compare data gainst existing
//	mDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	// Stencil operations if pixel is back-facing.
//	//keep if stencil test fails
//	mDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	//decrement if stencil and depth fails
//	mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	//keep if stencil test passes
//	mDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	//always compare data gainst existing
//	mDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	// Create the depth stencil state.
//	HRESULT result = md3dDevice->CreateDepthStencilState(&mDepthStencilDesc, &mDepthStencilState);
//	if (FAILED(result))
//	{
//		//like oh no scoob
//		//return false;
//	}
//
//	// Set the depth stencil state.
//	md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 1);
//
//	// Clear the second depth stencil state before setting the parameters.
//	ZeroMemory(&mDepthDisabledStencilDesc, sizeof(mDepthDisabledStencilDesc));
//
//	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
//	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
//	mDepthDisabledStencilDesc.DepthEnable = false;
//	mDepthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//	mDepthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//	mDepthDisabledStencilDesc.StencilEnable = true;
//	mDepthDisabledStencilDesc.StencilReadMask = 0xFF;
//	mDepthDisabledStencilDesc.StencilWriteMask = 0xFF;
//	mDepthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	mDepthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//	mDepthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	mDepthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//	mDepthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//	mDepthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//	mDepthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//	mDepthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//
//	// Create the state using the device.
//	result = md3dDevice->CreateDepthStencilState(&mDepthDisabledStencilDesc, &mDepthDisabledStencilState);
//	if (FAILED(result))
//	{
//		//return false;
//	}
//
//#pragma endregion DepthStencilStates
//
//}
//
//void FontDemo::DrawScene()
//{
//
//#pragma region InitialRenderingSetup
//	//do we have the context and swapchain?
//	assert(md3dImmediateContext);
//	assert(mSwapChain);
//
//	//set the effect techniques we wish to use
//	ID3DX11EffectTechnique* activeSpriteTech = DPhoenix::Effects::SpriteFX->SpriteTech;
//
//	//clear the screen with the defined colour
//	float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
//	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, clearColor);
//	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//	//set up the vertex input layout
//	//(would need to change for different types of rendering)
//	md3dImmediateContext->IASetInputLayout(DPhoenix::InputLayouts::SpritePosTex);
//	//set primitive topology as triangle list (adjacent trinagles to render for GPU efficiency)
//	md3dImmediateContext->IASetPrimitiveTopology(
//		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	//clear all Direct2D before rendering any text
//	//(this overwrites the 3D clear color)
//	m2D1RT->BeginDraw();
//
//	m2D1RT->Clear(D2D1::ColorF(0x00000000));
//
//	m2D1RT->EndDraw();
//
//#pragma endregion InitialRenderingSetup
//
//#pragma region RenderSprites
//
//	//to render sprites we need to disable depth testing (so it layers on top of each other)
//	//like a collage
//	md3dImmediateContext->OMSetDepthStencilState(mDepthDisabledStencilState, 1);
//
//	//render the sprite
//	//(we'll consider it a HUD object for now as we'll position by the viewport
//	//rather than camera matrices)
//	RenderSprite(mCharacterSprite, activeSpriteTech, true);
//
//	//turn depth stencil back on for 3D rendering
//	md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 1);
//
//#pragma endregion RenderSprites
//
//#pragma region RenderFonts
//
//	//string stream object for getting all text to display
//	std::ostringstream debugText;
//
//	debugText << "Debug text:" << std::endl;
//	debugText << "Position x: " << mCharacterSprite->mPosition.x << " | y: " 
//		<< mCharacterSprite->mPosition.y << std::endl;
//	debugText << "Velocity x: " << mCharacterSprite->mVelocity.x << " | y: " 
//		<< mCharacterSprite->mVelocity.y << std::endl;
//	debugText << "Flip Value: " << mCharacterSprite->mFlipValue << std::endl;
//
//	//calculate screen scaling
//	float scaleX = (float)mClientWidth / 1440;
//	float scaleY = (float)mClientHeight / 900;
//
//	//call draw font method sending all key info
//	mDebugFont->DrawFont(m2D1RT, debugText.str(), 10.0f, 500.0f,
//		1.0f, 1.0f, D2D1::ColorF(0xFFFFFF, 0.5f));
//
//#pragma endregion RenderFonts
//
//	//present to screen
//	HR(mSwapChain->Present(0, 0));
//
//}
//
//void FontDemo::HandleEvents(DPhoenix::IEvent* e)
//{
//	switch (e->GetID())
//	{
//	case DPhoenix::EVENT_KEYPRESS:
//	{
//		DPhoenix::KeyPressEvent* kpe = (DPhoenix::KeyPressEvent*)e;
//		switch (kpe->mKeycode)
//		{
//		case DIK_D:
//			mVelocity.x = 500.0f;
//			mCharacterSprite->mFlipValue = 1.0f;
//			mCharacterSprite->mAnimationDirection = 16;
//			break;
//		case DIK_A:
//			mVelocity.x = -500.0f;
//			mCharacterSprite->mFlipValue = -1.0f;
//			mCharacterSprite->mAnimationDirection = -16;
//			break;
//		case DIK_W:
//			mVelocity.y = 500.0f;
//			break;
//		case DIK_S:
//			mVelocity.y = -500.0f;
//			break;
//		case DIK_ESCAPE:
//			//shutdown here
//			break;
//		}
//	}
//	break;
//	case DPhoenix::EVENT_XBOX_INPUT:
//	{
//		DPhoenix::XBoxEvent* xbe = (DPhoenix::XBoxEvent*)e;
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
//		{
//			mVelocity.x = -500.0f;
//			mCharacterSprite->mFlipValue = -1.0f;
//			mCharacterSprite->mAnimationDirection = -16;
//		}
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
//		{
//			mVelocity.x = 500.0f;
//			mCharacterSprite->mFlipValue = 1.0f;
//			mCharacterSprite->mAnimationDirection = 16;
//		}
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
//		{
//			mVelocity.y = 500.0f;
//		}
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
//		{
//			mVelocity.y = -500.0f;
//		}
//		//audio buttons
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
//		{
//			if (!mAudioMgr.GetSound("IgnoranceTitle")->IsPlaying())
//			{
//				mAudioMgr.GetSound("IgnoranceTitle")->Play(true);
//			}
//
//		}
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_B)
//		{
//			if (!mAudioMgr.GetSound("LaserShootFX")->IsPlaying())
//			{
//				mAudioMgr.GetSound("LaserShootFX")->Play(false);
//			}
//			else
//			{
//				mAudioMgr.GetSound("LaserShootFX")->Stop();
//			}
//		}
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_X)
//		{
//			if (!mAudioMgr.GetSound("FlatBelieveClip")->IsPlaying())
//			{
//				mAudioMgr.GetSound("FlatBelieveClip")->Play(false);
//			}
//			else
//			{
//				mAudioMgr.GetSound("FlatBelieveClip")->Stop();
//			}
//		}
//		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
//		{
//			mAudioMgr.ResetAllSounds();
//		}
//	}
//	break;
//	}
//}
//
//void FontDemo::InitAudio()
//{
//	mAudioMgr.CreateSound("IgnoranceTitle", "Audio\\Music\\IgnoranceTitle.wav");
//	mAudioMgr.CreateSound("LaserShootFX", "Audio\\FX\\LaserShootFX.wav");
//	mAudioMgr.CreateSound("FlatBelieveClip", "Audio\\Voice\\FlatBelieveClip.wav");
//
//}
//
//void FontDemo::RenderSprite(DPhoenix::Sprite * sprite, ID3DX11EffectTechnique * tech, bool isHUD)
//{
//	//here we calculate the scale on X and Y based on the client width / height
//	//vs. our original resolution 
//	float scaleX = mScreenScale.x;
//	float scaleY = mScreenScale.y;
//
//	//add to a vector to use in matrices
//	XMFLOAT3 scaleVec; scaleVec.x = scaleX;
//	scaleVec.y = scaleY; scaleVec.z = 1.0f;
//
//	//create view / projection matrix
//	XMMATRIX viewProj;
//
//	//HUD objects not affected by cameras
//	if (!isHUD)
//	{
//		//here you would add any 2D camera control
//	}
//	else
//	{
//		//an identity matrix is standard and used as a basis
//		//for other calculations
//		XMMATRIX view = XMMatrixIdentity();
//		//orthographic, left handed co-ordinates
//		XMMATRIX projection = XMMatrixOrthographicLH(
//			(float)mClientWidth,	//width of viewport
//			(float)mClientHeight,	//height of viewport
//			0.1f,					//nearplane (Z)
//			100.0f					//far plane (Z)
//		);
//
//		//create view / projection matrix (multiply / concatenate)
//		//these represent the 'camera'
//		viewProj = XMMatrixMultiply(view, projection);
//	}
//
//	//concatenate world / view / projection matrices for rendering
//	XMMATRIX world = sprite->CalculateTransforms(scaleVec, isHUD);
//	XMMATRIX worldViewProj = XMMatrixMultiply(world, viewProj);
//	DPhoenix::Effects::SpriteFX->SetWorldViewProj(worldViewProj);
//
//	//now we get all the matching Sprite properties to send to the shader
//	DPhoenix::Effects::SpriteFX->SetDiffuseMap(sprite->GetShaderResourceView());			//texture
//	DPhoenix::Effects::SpriteFX->SetColumn(sprite->GetCurrentFrame());						//col / frame
//	DPhoenix::Effects::SpriteFX->SetNumCols(sprite->mAnimationColumns);						//num frames
//	DPhoenix::Effects::SpriteFX->SetFlipValue(sprite->mFlipValue);							//flip value
//	DPhoenix::Effects::SpriteFX->SetLerpValue(sprite->mColorLerpValue);						//lerp value
//	DPhoenix::Effects::SpriteFX->SetOpacityValue(sprite->mOpacityValue);					//opacity
//	DPhoenix::Effects::SpriteFX->SetAltColorValue(XMLoadFloat4(&sprite->mAltColorValue));	//alt color
//
//																							//render using effect shader technique
//	D3DX11_TECHNIQUE_DESC techDesc;
//	tech->GetDesc(&techDesc);
//	for (UINT p = 0; p < techDesc.Passes; ++p)
//	{
//		//use current context and get current rendering pass
//		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
//
//		//call Render method of Sprite object
//		sprite->Render(md3dImmediateContext);
//	}
//
//	//reset the lerp value on the shader
//	DPhoenix::Effects::SpriteFX->SetLerpValue(0.0f);
//}
//
