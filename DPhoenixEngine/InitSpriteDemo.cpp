//#include "Engine.h"
//
////initial class definition inheriting fron Engine
//class InitSpriteDemo : public DPhoenix::Engine
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
//	//sprite shader resource view pointer (texture)
//	ID3D11ShaderResourceView* mSpriteTexSRV;
//
//	//sprite position
//	XMFLOAT3 position;
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
//	InitSpriteDemo(HINSTANCE hInstance);
//	~InitSpriteDemo();
//
//	bool Init(bool fullScreen);
//	void OnResize();
//	void UpdateScene(float dt);
//	void DrawScene();
//
//	//new function; this builds our quad for the sprite
//	void BuildGeometryBuffers();
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
//	InitSpriteDemo theApp(hInstance);
//
//	if (!theApp.Init(false))
//		return 0;
//
//	return theApp.Run();
//}
//
////constructor - don't really need to add much here
//InitSpriteDemo::InitSpriteDemo(HINSTANCE hInstance)
//	: Engine(hInstance)
//{
//	mMainWndCaption = "Initial Sprite Demo";
//
//
//}
//
////destructor (release any memory as necessary)
//InitSpriteDemo::~InitSpriteDemo()
//{
//	ReleaseCOM(mSpriteVB);
//	ReleaseCOM(mSpriteIB);
//
//	DPhoenix::Effects::DestroyAll();
//	DPhoenix::InputLayouts::DestroyAll();
//}
//
////any additional initialisation (we generally initiaise game assets here)
//bool InitSpriteDemo::Init(bool fullScreen)
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
//	//create the texture with the texture manager and store pointer
//	mSpriteTexSRV = mTexMgr.CreateTexture("Textures\\LifeHead.png");
//
//	//build quads for the sprite
//	BuildGeometryBuffers();
//
//	//set initial position
//	position.x = 5.0f; position.y = 5.0f; position.z = 0.0f;
//
//	return true;
//}
//
////what to do on resize - mainly we'lll just call the parent / engine's method
//void InitSpriteDemo::OnResize()
//{
//	Engine::OnResize();
//}
//
////update scene - here is where our update logic for the game loop goes
//void InitSpriteDemo::UpdateScene(float dt)
//{
//	//we are using the AsyncKeyState until we set up
//	//our DirectInput events
//	//basic WASD conrol for horiz / vert movement
//	if (GetAsyncKeyState('A') & 0x8000)
//		position.x -= 500.0f * dt;
//	if (GetAsyncKeyState('D') & 0x8000)
//		position.x += 500.0f * dt;
//	if (GetAsyncKeyState('W') & 0x8000)
//		position.y += 500.0f * dt;
//	if (GetAsyncKeyState('S') & 0x8000)
//		position.y -= 500.0f * dt;
//}
//
//void InitSpriteDemo::BuildGeometryBuffers()
//{
//#pragma region GeometryCreation
//
//
//	//here we set the width and height of the quad
//	//(match texture dimensions)
//	float halfWidth = 50.0f;
//	float halfHeight = 50.0f;
//
//	//A quad with UV co-ords
//	Vertex::SpritePosTex vertices[] =
//	{
//		//A											//UV1
//		{ XMFLOAT3(halfWidth, halfHeight, 1.0f),	XMFLOAT2(1.0f, 0.0f) },
//		//B											//UV2
//		{ XMFLOAT3(halfWidth, -halfHeight, 1.0f),	XMFLOAT2(1.0f, 1.0f) },
//		//C											//UV3
//		{ XMFLOAT3(-halfWidth, -halfHeight, 1.0f),	XMFLOAT2(0.0f, 1.0f) },
//		//D											//UV4
//		{ XMFLOAT3(-halfWidth, halfHeight, 1.0f),	XMFLOAT2(0.0f, 0.0f) }
//	};
//
//	//create vertex buffer
//	D3D11_BUFFER_DESC vbd;								//desc struct for vertex buffer
//	vbd.Usage = D3D11_USAGE_IMMUTABLE;					//usage flags (can't edit but quicker)
//	vbd.ByteWidth = sizeof(Vertex::SpritePosTex) * 4;	//four vertices so multiply size by 4
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//bind as vertex buffer
//	vbd.CPUAccessFlags = 0;								//CPU access flags
//	vbd.MiscFlags = 0;									//Misc flags
//	vbd.StructureByteStride = 0;						//this goes pretty deep! Lets leave at 0...
//	//create subresource with vertex buffer data
//	D3D11_SUBRESOURCE_DATA vinitData;
//	//apply vertices array
//	vinitData.pSysMem = vertices;
//	//create buffer from device (stored in mSpriteVB)
//	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSpriteVB));
//
//	// Create the index buffer
//	//(two triangles from teh four vertices)
//	UINT indices[] = {
//		// front face
//		0, 1, 2,
//		0, 2, 3,
//	};
//
//	//create index buffer subresource
//	D3D11_BUFFER_DESC ibd;								//desc struct for vertex buffer
//	ibd.Usage = D3D11_USAGE_IMMUTABLE;					//immutable as the box will always be a box
//	ibd.ByteWidth = sizeof(UINT) * 6;					//6 indices
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;			//Bind as index buffer
//	ibd.CPUAccessFlags = 0;								//CPU access flags
//	ibd.MiscFlags = 0;									//Misc flags
//	ibd.StructureByteStride = 0;						//this goes pretty deep! Lets leave at 0...
//	//create subresource with index buffer data
//	D3D11_SUBRESOURCE_DATA iinitData;
//	//apply indices array
//	iinitData.pSysMem = indices;
//	//create buffer from device (stored in mSpriteIB)
//	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSpriteIB));
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
//	//this can affect the blend / transparency with different hex values
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
//void InitSpriteDemo::DrawScene()
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
//#pragma endregion InitialRenderingSetup
//
//#pragma region RenderSprites
//
//	//to render sprites we need to disable depth testing (so it layers on top of each other)
//	//like a collage
//	md3dImmediateContext->OMSetDepthStencilState(mDepthDisabledStencilState, 1);
//
//	//the stride is how many bytes from one bit of data to another
//	UINT stride = sizeof(Vertex::SpritePosTex);
//	//the offset is 0; start at the beginning
//	UINT offset = 0;
//	//set the vertex buffer
//	md3dImmediateContext->IASetVertexBuffers(
//		0, 1, &mSpriteVB, &stride, &offset
//	);
//	//set the index buffer
//	md3dImmediateContext->IASetIndexBuffer(
//		mSpriteIB, DXGI_FORMAT_R32_UINT, 0
//	);
//
//	//an identity matrix is standard and used as a basis
//	//for other calculations
//	XMMATRIX view = XMMatrixIdentity();
//
//	//here we calculate the scale on X and Y based on the client width / height
//	//vs. our original resolution 
//	float scaleX = (float)mClientWidth / 1440;
//	float scaleY = (float)mClientHeight / 900;
//
//	//add to a vector to use in matrices
//	XMFLOAT3 scaleVec; scaleVec.x = scaleX;
//	scaleVec.y = scaleY; scaleVec.z = 1.0f;
//	
//	//orthographic, left handed co-ordinates
//	XMMATRIX projection = XMMatrixOrthographicLH(
//		(float)mClientWidth,	//width of viewport
//		(float)mClientHeight,	//height of viewport
//		0.1f,					//nearplane (Z)
//		100.0f					//far plane (Z)
//	);
//
//	//create view / projection matrix (multiply / concatenate)
//	//these represent the 'camera'
//	XMMATRIX viewProj = XMMatrixMultiply(view, projection);
//
//	//Set constants (matrices for rendering)
//	//translate - mtrix for positioning
//	XMMATRIX translate = XMMatrixTranslation(position.x, position.y, position.z);
//	//scale - matrix for sizing
//	XMMATRIX scale = XMMatrixScaling(scaleVec.x, scaleVec.y, scaleVec.z);
//	//rotation (around Z axis as face on to camera)
//	XMMATRIX rotate = XMMatrixRotationZ(0.0f);
//	//concatenate matreices (TRS order) to byuild world matrix for sprite
//	XMMATRIX world = translate * rotate * scale;
//
//	//concatenate world / view / projection matrices for rendering
//	XMMATRIX worldViewProj = XMMatrixMultiply(world, viewProj);
//	//send to shader
//	DPhoenix::Effects::SpriteFX->SetWorldViewProj(worldViewProj);
//	//send texture to sahder (SRV)
//	DPhoenix::Effects::SpriteFX->SetDiffuseMap(mSpriteTexSRV);
//
//	//render using effect shader technique
//	D3DX11_TECHNIQUE_DESC techDesc;
//	activeSpriteTech->GetDesc(&techDesc);
//	for (UINT p = 0; p < techDesc.Passes; ++p)
//	{
//		//use current context and get current rendering pass
//		activeSpriteTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
//
//		//6 indices for quad
//		md3dImmediateContext->DrawIndexed(6, 0, 0);
//	}
//
//	//turn depth stencil back on for 3D rendering
//	md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 1);
//
//#pragma endregion RenderSprites
//
//	//present to screen
//	HR(mSwapChain->Present(0, 0));
//
//}
//
