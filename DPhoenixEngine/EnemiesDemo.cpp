#include "Engine.h"
#include "DeanyP.h"
#include "Map.h"
#include "Camera2D.h"
#include "Collectable.h"
#include "Fire.h"
#include "ChemPlane.h"
#include "Angel.h"
#include "AntiVax.h"

enum GameStates
{
	GAME_TITLE_STATE,
	GAME_PLAY_STATE,
	GAME_PAUSE_STATE,
	GAME_OVER_STATE,
	GAME_WIN_STATE
};

//initial class definition inheriting fron Engine
class EnemiesDemo : public DPhoenix::Engine
{
private:
	//depth stencil (to handle multiple overlayed sprites)
	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11DepthStencilState* mDepthDisabledStencilState;
	D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;
	D3D11_DEPTH_STENCIL_DESC mDepthDisabledStencilDesc;

	//texture manager
	DPhoenix::TextureMgr mTexMgr;

	//audio manager
	DPhoenix::AudioMgr mAudioMgr;

	//font pointer - debug text
	DPhoenix::Font* mDebugFont;

	//screen scale (for window stretching)
	XMFLOAT2 mScreenScale;

#pragma region GameMembers

	GameStates mGameState;

	DPhoenix::Sprite* mBGSprite;
	DPhoenix::DeanyP* mDeanyP;
	DPhoenix::Sprite* mTitleSprite;

	bool mStartPressedFlag;
	bool mStartWasPressedFlag;

	DPhoenix::Map* mMap;
	DPhoenix::Camera2D* mCamera;

	std::vector<DPhoenix::Fire*> mPlayerFireballs;
	std::vector<DPhoenix::Collectable*> mCollectablesVec;

	//vectors for enemies
	std::vector<DPhoenix::Fire*> mEnemyFireballs;
	std::vector<DPhoenix::Enemy*> mEnemiesVec;
	std::vector<std::vector<XMFLOAT2*>> mEnemySpawnVec;
	std::vector<DPhoenix::Angel*> mAngelsVec;
	//values for enemies killed / total
	int mAngelsCollected;
	int mAngelsTotal;

	//sprites for win / lose states
	DPhoenix::Sprite* mWinSprite;
	DPhoenix::Sprite* mGameOverSprite;

#pragma endregion GameMembers

#pragma region HUDMembers


	DPhoenix::Sprite* mHUDKnowlegdeLabel;	//knowledge animation
	DPhoenix::Sprite* mHUDBackCog;			//cog - max knowledge
	DPhoenix::Sprite* mHUDFrontCog;			//cog - current knowlegde
	DPhoenix::Sprite* mHUDHeart;			//heart animation
	DPhoenix::Sprite* mDeanyHealthBG;		//health bar background (max health)
	DPhoenix::Sprite* mDeanyHealth;			//health bar (current health)

	XMFLOAT4 mHUDHeartAltColor;				//alt colour for heart
	float mHUDHeartLerpValue;				//lerp value for alt color

	float mCogRotationValue;				//value for rotating cogs
	XMFLOAT4 mHUDFrontCogAltColor;			//alt colour for front cog
	float mHUDFrontCogLerpValue;			//lerp value for alt color	

#pragma endregion HUDMembers

public:
	EnemiesDemo(HINSTANCE hInstance);
	~EnemiesDemo();

	bool Init(bool fullScreen);
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	//new function; this builds our quad for the sprite
	void BuildGeometryBuffers();

	//new method / implements event handling
	void EnemiesDemo::HandleEvents(DPhoenix::IEvent* e);

	//new method / initialises sounds to be used in the app
	void InitAudio();

	//new method / specifically handles individual sprite rendering
	void RenderSprite(DPhoenix::Sprite* sprite,
		ID3DX11EffectTechnique* tech, bool isHUD = false);

	//methods for game
	void InitBackground();
	void RenderMap(ID3DX11EffectTechnique* tech);
	void ReInit();
};

//our epplication entry point - WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	EnemiesDemo theApp(hInstance);

	if (!theApp.Init(false))
		return 0;

	return theApp.Run();
}

//constructor - don't really need to add much here
EnemiesDemo::EnemiesDemo(HINSTANCE hInstance)
	: Engine(hInstance)
{
	mMainWndCaption = "Enemies Demo";

}

//destructor (release any memory as necessary)
EnemiesDemo::~EnemiesDemo()
{
	DPhoenix::Effects::DestroyAll();
	DPhoenix::InputLayouts::DestroyAll();
}

//any additional initialisation (we generally initiaise game assets here)
bool EnemiesDemo::Init(bool fullScreen)
{
	if (!Engine::Init(fullScreen))
		return false;

	//initialise the texture manager
	mTexMgr.Init(md3dDevice);

	// Must init Effects first since InputLayouts depend on shader signatures.
	DPhoenix::Effects::InitAll(md3dDevice);
	DPhoenix::InputLayouts::InitAll(md3dDevice);

	//must init soundlayer to then buffer audio
	DPhoenix::SoundLayer::Create(mhMainWnd);
	InitAudio();

	//build render / blend modes etc.
	BuildGeometryBuffers();

	//debug font
	//(Arial size 20 sending window, initial brush, writefactory)
	mDebugFont = new DPhoenix::Font(mhMainWnd, mBlackBrush,
		mDWriteFactory, "Arial", 20.0f);

#pragma region GameAssetsInit

	//map (the 2, 1 refers to 2 panels wide, 1 panel high)
	mMap = new DPhoenix::Map("Data\\Levels\\TestLevel.csv", &mTexMgr, md3dDevice,
		mCollectablesVec, mEnemySpawnVec,
		48, 30, 2, 1);

	//player
	mDeanyP = new DPhoenix::DeanyP(&mTexMgr, md3dDevice, &mAudioMgr);
	//set the player spawn point
	mDeanyP->mPosition = mMap->mPlayerSpawnPoint;

	//enemies ---------------------------------------------------------------------
	//loop through spawn points vector
	for (int i = 0; i < mEnemySpawnVec.size(); i++)
	{
		//cast to enemy types and check
		switch ((DPhoenix::EnemyTypesForMap)i)
		{
			//if chemplane type, loop through the containined vector to spawn
		case DPhoenix::ENEMYMAP_CHEMPLANE:
			for (int j = 0; j < mEnemySpawnVec[i].size(); j++)
			{
				//construct new ChemPlane enemy with spaen point, device pointers
				//and add to enemies vector to manage updates / rendering
				mEnemiesVec.push_back(new DPhoenix::ChemPlane(&mTexMgr, md3dDevice, 
					*mEnemySpawnVec[i][j], &mAudioMgr));
			}

			break;
		case DPhoenix::ENEMYMAP_ANTIVAX:
			for (int j = 0; j < mEnemySpawnVec[i].size(); j++)
			{
				//construct new ChemPlane enemy with spaen point, device pointers
				//and add to enemies vector to manage updates / rendering
				mEnemiesVec.push_back(new DPhoenix::AntiVax(&mTexMgr, md3dDevice,
					*mEnemySpawnVec[i][j], &mAudioMgr));
			}

			break;
		}
	}
	//set values for debug text (and eventually HUD?)
	mAngelsCollected = 0;
	mAngelsTotal = mEnemiesVec.size();

	mTitleSprite = new DPhoenix::Sprite();
	mTitleSprite->Load("Textures\\ObjectAssets\\TitleScreen.png", &mTexMgr,
		1440.0f, 900.0f, md3dDevice);
	mTitleSprite->SetCurrentFrame(0);	mTitleSprite->mAnimationColumns = 1;
	mTitleSprite->mAnimationDirection = 0;	mTitleSprite->SetAnimationRange(0, 0);
	mTitleSprite->SetScale(1.0f);	mTitleSprite->mFlipValue = 1.0f;

	//camera
	mCamera = new DPhoenix::Camera2D();
	mCamera->mPosition.x = 0.0f;  mCamera->mPosition.y = 0.0f;
	mCamera->mSpeed.x = 0.0f;  mCamera->mSpeed.y = 0.0f;
	//used to simplify bounding calculations
	float tileAdjust = mMap->mTileSize / 2;

	//set the camera bounding limits (horizontal)
	mCamera->mLeftLimit = (mMap->mPosition.x + mClientWidth / 2) - tileAdjust;
	mCamera->mRightLimit =
		mCamera->mLeftLimit + mMap->mWidth * mMap->mTileSize - mClientWidth;
	//(vertical)
	mCamera->mBottomLimit = (mMap->mPosition.y + mClientHeight / 2) - tileAdjust;
	mCamera->mTopLimit =
		mCamera->mBottomLimit + mMap->mHeight * mMap->mTileSize - mClientHeight;

	InitBackground();

	//win / level complete sprite
	mWinSprite = new DPhoenix::Sprite();
	mWinSprite->Load("Textures\\LevelAssets\\ResultsBanner.png", &mTexMgr, 900.0f, 400.0f, md3dDevice);
	mWinSprite->SetCurrentFrame(0);	mWinSprite->mAnimationColumns = 1;
	mWinSprite->mAnimationDirection = 0;	mWinSprite->SetAnimationRange(0, 0);
	mWinSprite->SetScale(1.0f);	mWinSprite->mFlipValue = 1.0f;
	//game over sprite
	mGameOverSprite = new DPhoenix::Sprite();
	mGameOverSprite->Load("Textures\\ObjectAssets\\GameOverBanner.png", &mTexMgr, 500.0f, 200.0f, md3dDevice);
	mGameOverSprite->SetCurrentFrame(0);	mGameOverSprite->mAnimationColumns = 1;
	mGameOverSprite->mAnimationDirection = 0;	mGameOverSprite->SetAnimationRange(0, 0);
	mGameOverSprite->SetScale(1.0f);	mGameOverSprite->mFlipValue = 1.0f;


	mAudioMgr.GetSound("IgnoranceTitle")->Play(true);

	mGameState = GAME_TITLE_STATE;

#pragma endregion GameAssetsInit

#pragma region HUDInit

	//HUD ------------------------------------------------------------------------
	mHUDKnowlegdeLabel = new DPhoenix::Sprite();
	mHUDKnowlegdeLabel->Load("Textures\\ObjectAssets\\KnowledgeLabel.png", &mTexMgr, 175.0f, 30.0f, md3dDevice);
	mHUDKnowlegdeLabel->SetCurrentFrame(0);	mHUDKnowlegdeLabel->mAnimationColumns = 12;
	mHUDKnowlegdeLabel->mAnimationDirection = 6;	mHUDKnowlegdeLabel->SetAnimationRange(0, 11);
	mHUDKnowlegdeLabel->SetScale(1.0f);	mHUDKnowlegdeLabel->mFlipValue = 1.0f;

	mHUDBackCog = new DPhoenix::Sprite();
	mHUDBackCog->Load("Textures\\ObjectAssets\\BackCog.png", &mTexMgr, 50.0f, 50.0f, md3dDevice);

	mHUDFrontCog = new DPhoenix::Sprite();
	mHUDFrontCog->Load("Textures\\ObjectAssets\\FrontCog.png", &mTexMgr, 50.0f, 50.0f, md3dDevice);
	//red is alt color value
	mHUDFrontCog->mAltColorValue = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	mCogRotationValue = 0.0f;
	mHUDFrontCogLerpValue = 0.0f;

	mHUDHeart = new DPhoenix::Sprite();
	mHUDHeart->Load("Textures\\ObjectAssets\\Heart.png", &mTexMgr, 50.0f, 50.0f, md3dDevice);
	mHUDHeart->SetCurrentFrame(0);	mHUDHeart->mAnimationColumns = 4;
	mHUDHeart->mAnimationDirection = 10;	mHUDHeart->SetAnimationRange(0, 3);
	mHUDHeart->SetScale(1.0f);	mHUDHeart->mFlipValue = 1.0f;
	//grey is alt color value
	mHUDHeart->mAltColorValue = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	mHUDHeartLerpValue = 0.0f;

	mDeanyHealthBG = new DPhoenix::Sprite();
	mDeanyHealthBG->Load("Textures\\ObjectAssets\\HealthBar.png", &mTexMgr, 100.0f, 10.0f, md3dDevice);
	mDeanyHealthBG->SetCurrentFrame(0);	mDeanyHealthBG->mAnimationColumns = 1;
	mDeanyHealthBG->mAnimationDirection = 0;	mDeanyHealthBG->SetAnimationRange(0, 0);
	mDeanyHealthBG->SetScale(1.0f);	mDeanyHealthBG->mFlipValue = 1.0f;

	mDeanyHealth = new DPhoenix::Sprite();
	mDeanyHealth->Load("Textures\\ObjectAssets\\HealthBarGreen.png", &mTexMgr, 100.0f, 10.0f, md3dDevice);
	mDeanyHealth->SetCurrentFrame(0);	mDeanyHealth->mAnimationColumns = 1;
	mDeanyHealth->mAnimationDirection = 0;	mDeanyHealth->SetAnimationRange(0, 0);
	mDeanyHealth->SetScale(1.0f);	mDeanyHealth->mFlipValue = 1.0f;

#pragma endregion HUDInit

	return true;
}

//what to do on resize - mainly we'll just call the parent / engine's method
void EnemiesDemo::OnResize()
{
	Engine::OnResize();
}

//update scene - here is where our update logic for the game loop goes
void EnemiesDemo::UpdateScene(float dt)
{
	//calculate scaling based on window dimensions
	mScreenScale.x = (float)mClientWidth / 1440;
	mScreenScale.y = (float)mClientHeight / 900;

	mStartWasPressedFlag = mStartPressedFlag;
	mStartPressedFlag = false;

	switch (mGameState)
	{
	case GAME_TITLE_STATE:
	{
		//keep playing title screen
		mTitleSprite->Update(dt);
	}
	break;
	case GAME_PLAY_STATE:
	{
		//camera and movment updates to go here
		//and collision testing for assets

		mBGSprite->Update(dt);

		//call update for character movement
		mDeanyP->CharacterUpdate(dt, mMap);

#pragma region CameraUpdates

		//UNCOMMENT TO FOCUS ON CHARACTER --------------
		mCamera->mPosition.x = mDeanyP->mPosition.x;
		mCamera->mPosition.y = mDeanyP->mPosition.y;

		mCamera->Update(dt, mClientWidth, mClientHeight);

#pragma endregion CameraUpdates

#pragma region PlayerFireUpdates

		//if flag set for deany p to fire
		if (mDeanyP->mToFireFlag)
		{
			//create new fireball and push back in player fire vector
			mPlayerFireballs.push_back
			(new DPhoenix::Fire(&mTexMgr, md3dDevice,
				mDeanyP->mPosition,
				mDeanyP->mFireDirections[mDeanyP->mCurrentFireDirection],
				DPhoenix::FIRE_PLAYER,
				&mAudioMgr));
			//(as well as pointers to main engine classes, we send the
			//current position, current fire direction and fire type)
		}
		//loop through player fireballs vector
		for (int i = 0; i < mPlayerFireballs.size();)
		{
			//update the fireball
			mPlayerFireballs[i]->Update(dt);
			//get current position
			XMFLOAT2 fbPos = mPlayerFireballs[i]->mPosition;

			//Calculate when fireballs offscreen
			//(magic numbers here but could be replaced for halfsize values
			//I'm coding here for convenience and speed)
			if (fbPos.x + 10.0f < mCamera->mPosition.x - mClientWidth / 2 ||
				fbPos.x - 10.0f > mCamera->mPosition.x + mClientWidth / 2 ||
				fbPos.y + 10.0f < mCamera->mPosition.y - mClientHeight / 2 ||
				fbPos.y - 10.0f > mCamera->mPosition.y + mClientHeight / 2)
			{
				//we are offscreen so remove 
				//(this keeps i at the current value as vector size shifts)
				mPlayerFireballs.erase(mPlayerFireballs.begin() + i);
			}
			else
			{
				//otherwise increment the iterator value
				i++;
			}
		}
#pragma endregion PlayerFireUpdates

#pragma region CollectablesUpdates

		//loop through collectables vector
		for (int i = 0; i < mCollectablesVec.size();)
		{
			//update the collectable
			mCollectablesVec[i]->Update(dt);
			//flag for removal following checks
			bool remove = false;
			//check if a remove flag is set (timer based)
			if (mCollectablesVec[i]->mRemoveFlag)
			{
				//flag for removal
				remove = true;
			}
			//check if teh player AABB overlaps the collectable AABB
			if (mDeanyP->mAABB.Overlaps(mCollectablesVec[i]->mAABB))
			{
				//flag for removal
				remove = true;
				//check the collectable type
				switch (mCollectablesVec[i]->mCollectableType)
				{
				case DPhoenix::COLLECTABLE_RESEARCH:
					mDeanyP->mMaxKnowledge += 10;	//update max knowledge
													//bound at max 200
					if (mDeanyP->mMaxKnowledge > 200) mDeanyP->mMaxKnowledge = 200;
					//could play sound here
					break;
				case DPhoenix::COLLECTABLE_COFFEE:
					mDeanyP->mHealth += 10;		//update health
												//bound at max health
					if (mDeanyP->mHealth > mDeanyP->mMaxHealth)
						mDeanyP->mHealth = mDeanyP->mMaxHealth;
					//could play sound here
					break;
				}
			}
			//if flagged for removal
			if (remove)
			{
				//remove
				//(this keeps i at the current value as vector size shifts)
				mCollectablesVec.erase(mCollectablesVec.begin() + i);
			}
			else
			{
				//otherwise increment the iterator value
				i++;
			}

		}

#pragma endregion CollectablesUpdates

#pragma region EnemyFireUpdates

		//loop through enemy fireballs vector
		for (int i = 0; i < mEnemyFireballs.size();)
		{
			//update animations / positions etc.
			mEnemyFireballs[i]->Update(dt);
			//get current position
			XMFLOAT2 fbPos = mEnemyFireballs[i]->mPosition;
			//flag for removal
			bool removeFlag = false;

			//magic numbers here but could be replaced for halfsize values
			//I'm coding here for convenience and speed
			//if offscreen remove
			if (fbPos.x + 10.0f < mCamera->mPosition.x - mClientWidth / 2 ||
				fbPos.x - 10.0f > mCamera->mPosition.x + mClientWidth / 2 ||
				fbPos.y + 10.0f < mCamera->mPosition.y - mClientHeight / 2 ||
				fbPos.y - 10.0f > mCamera->mPosition.y + mClientHeight / 2)
			{
				//we are offscreen so remove 
				removeFlag = true;
			}
			//if Deany P not dying and fireball collides with Deany P's hitbox
			//and isn't 'friendly fire' then take damage and flag for removal
			if (mDeanyP->mLifeState != DPhoenix::DP_DIE_LIFESTATE &&
				mDeanyP->mAABB.Overlaps(mEnemyFireballs[i]->mAABB)
				&& !mEnemyFireballs[i]->mIsFriendlyFire)
			{
				mDeanyP->TakeDamage(5);
				removeFlag = true;
			}
			//if flagged for removal, erase fire from vector, otherwise incremnet iterator
			if (removeFlag)
			{
				mEnemyFireballs.erase(mEnemyFireballs.begin() + i);
			}
			else
			{
				i++;
			}
		}

#pragma endregion EnemyFireUpdates

#pragma region EnemiesUpdates

		//loop through enemies vector
		for (int i = 0; i < mEnemiesVec.size();)
		{
			//if the position of the enemy is onscreen 
			//(+ 100px each way offscreen)
			if (mEnemiesVec[i]->mPosition.x + 100.0f > 
					mCamera->mPosition.x - mClientWidth / 2 &&
				mEnemiesVec[i]->mPosition.x - 100.0f < 
					mCamera->mPosition.x + mClientWidth / 2 &&
				mEnemiesVec[i]->mPosition.y + 100.0f > 
					mCamera->mPosition.y - mClientHeight / 2 &&
				mEnemiesVec[i]->mPosition.y - 100.0f <
					mCamera->mPosition.y + mClientHeight / 2)
			{
				//we are in range so activate 
				if (!mEnemiesVec[i]->mIsActive)
				{
					mEnemiesVec[i]->mIsActive = true;
					mEnemiesVec[i]->ResetTimers();
				}
			}
			//if the position of the enemy is greater than 500px offscreen
			if (mEnemiesVec[i]->mPosition.x + 500.0f < 
					mCamera->mPosition.x - mClientWidth / 2 ||
				mEnemiesVec[i]->mPosition.x - 500.0f > 
					mCamera->mPosition.x + mClientWidth / 2 ||
				mEnemiesVec[i]->mPosition.y + 500.0f < 
					mCamera->mPosition.y - mClientHeight / 2 ||
				mEnemiesVec[i]->mPosition.y - 500.0f > 
					mCamera->mPosition.y + mClientHeight / 2)
			{
				//we are out of range so deactivate 
				mEnemiesVec[i]->mPosition = mEnemiesVec[i]->mInitialSpawnPos;
				mEnemiesVec[i]->mIsActive = false;
			}
			//update logic for enemies
			mEnemiesVec[i]->EnemyUpdate(dt, mMap, mDeanyP->mPosition);
			//if the enemy is a hazard and hitbox collides with DeanyP, take damage
			if (mEnemiesVec[i]->isHazard && mEnemiesVec[i]->mAABB.Overlaps(mDeanyP->mAABB))
			{
				mDeanyP->TakeDamage(5);
			}
			//if the enemy is flagged for firing
			if (mEnemiesVec[i]->mToFireFlag)
			{
				//get fire position from enemy member vars
				XMFLOAT2 firePosition;
				firePosition.x = mEnemiesVec[i]->mPosition.x + mEnemiesVec[i]->mFireOffset.x;
				firePosition.y = mEnemiesVec[i]->mPosition.y + mEnemiesVec[i]->mFireOffset.y;

				//construct new fire of type set, from position, direction and device pointers
				//add to enmey fireballs vector
				mEnemyFireballs.push_back(new DPhoenix::Fire(&mTexMgr, md3dDevice,
					firePosition,
					mEnemiesVec[i]->mFireDirection,
					mEnemiesVec[i]->mFireType, &mAudioMgr));
			}
			//nested loop to iterate through player fireballs
			for (int j = 0; j < mPlayerFireballs.size();)
			{
				//if the fireball overlaps with current enemy, take damage and erase fireball
				//otherwise keep increment iterator for nested loop
				if (mEnemiesVec[i]->mAABB.Overlaps(mPlayerFireballs[j]->mAABB))
				{
					//separate check in TakeDamage to check for weakpoints
					mEnemiesVec[i]->TakeDamage(10, mPlayerFireballs[j]->mAABB);
					mPlayerFireballs.erase(mPlayerFireballs.begin() + j);
				}
				else
				{
					j++;
				}
			}
			//if enemy flagged for removal, add angel from position, erase enemy and increment
			//'angels collected', otherwise increment iterator
			if (mEnemiesVec[i]->mRemoveFlag)
			{
				mAngelsVec.push_back(new DPhoenix::Angel(&mTexMgr, md3dDevice,
					mEnemiesVec[i]->mPosition, XMFLOAT2(0.0f, 1.0f)));
				mEnemiesVec.erase(mEnemiesVec.begin() + i);
				mAngelsCollected++;
			}
			else
			{
				i++;
			}
		}
#pragma endregion EnemiesUpdates

#pragma region AngelsUpdates

		//loop through angels vector
		for (int i = 0; i < mAngelsVec.size();)
		{
			//update logic
			mAngelsVec[i]->Update(dt);

			XMFLOAT2 angelPos = mAngelsVec[i]->mPosition;

			//magic numbers here but could be replaced for halfsize values
			//I'm coding here for convenience and speed
			if (angelPos.x + 10.0f < mCamera->mPosition.x - mClientWidth / 2 ||
				angelPos.x - 10.0f > mCamera->mPosition.x + mClientWidth / 2 ||
				angelPos.y + 10.0f < mCamera->mPosition.y - mClientHeight / 2 ||
				angelPos.y - 10.0f > mCamera->mPosition.y + mClientHeight / 2)
			{
				//we are offscreen so remove 
				mAngelsVec.erase(mAngelsVec.begin() + i);
			}
			else
			{
				i++;
			}
		}

#pragma endregion AngelsUpdates

#pragma region HUDUpdates

		//HUD Updates -----------------------------------------------------------------------------

		//calculate position for knowledge label 20px offset from top-left
		XMFLOAT2 knowledgeLabelPos;
		knowledgeLabelPos.x = (-mClientWidth / 2 + mHUDKnowlegdeLabel->mSize.x / 2) + 20.0f;
		knowledgeLabelPos.y = (mClientHeight / 2 - mHUDKnowlegdeLabel->mSize.y / 2) - 20.0f;
		//set the sprite position to calculated position
		mHUDKnowlegdeLabel->mPosition.x = knowledgeLabelPos.x;
		mHUDKnowlegdeLabel->mPosition.y = knowledgeLabelPos.y;
		//update animation
		mHUDKnowlegdeLabel->Update(dt);

		//cogs -------------------------------------------------------------------------------------
		XMFLOAT2 cogPos;
		//calculate cog position where centre is 70px underneath the knowledge label
		cogPos.x = knowledgeLabelPos.x;	cogPos.y = knowledgeLabelPos.y - 70.0f;
		//set both back and front cog to the calculated position
		mHUDFrontCog->mPosition.x = cogPos.x; mHUDFrontCog->mPosition.y = cogPos.y;
		mHUDBackCog->mPosition.x = cogPos.x; mHUDBackCog->mPosition.y = cogPos.y;
		//update cog rotation value (lower speed with less knowledge %)
		mCogRotationValue += ((float)mDeanyP->mKnowledge / mDeanyP->mMaxKnowledge) * dt * 3.0f;
		//set rotation speeds (font and back cog rotate opposite directions)
		mHUDFrontCog->mRotation = -mCogRotationValue;
		mHUDBackCog->mRotation = mCogRotationValue;
		//calculate the scale value for back cog 
		float backCogScale = mDeanyP->mMaxKnowledge / 100.0f;
		//calculate scale of front cog (as % of overall / max based on knowledge value)
		float frontCogScale = ((float)mDeanyP->mKnowledge / mDeanyP->mMaxKnowledge) * backCogScale;
		//adjust lerp value to subtract from 1 the % of knowledge from max
		mHUDFrontCogLerpValue = 1.0f - ((float)mDeanyP->mKnowledge / mDeanyP->mMaxKnowledge);
		//set lerp value as calculated
		mHUDFrontCog->mColorLerpValue = mHUDFrontCogLerpValue;
		//set the cog scales as calculated
		mHUDFrontCog->SetScale(frontCogScale);
		mHUDBackCog->SetScale(backCogScale);
		//update the cogs
		mHUDFrontCog->Update(dt);
		mHUDBackCog->Update(dt);

		//heart ------------------------------------------------------------------------------------
		XMFLOAT2 heartPos;
		//calculate heart pos 30px to right of knowledge label
		heartPos.x = knowledgeLabelPos.x + mHUDKnowlegdeLabel->mSize.x / 2 + 30.0f;
		heartPos.y = knowledgeLabelPos.y;
		//set lerp value to subtract from 1 the % of health from max health
		mHUDHeartLerpValue = 1.0f - ((float)mDeanyP->mHealth / mDeanyP->mMaxHealth);
		//set lerp value as calculated
		mHUDHeart->mColorLerpValue = mHUDHeartLerpValue;
		//calculate % of health from max
		float HeartPercent = ((float)mDeanyP->mHealth / mDeanyP->mMaxHealth);
		//based on %, adjust heart direction (beats slower with lower health)
		int HeartAnimationDirection = 2 + (int)(HeartPercent * 10);
		//set animation direction as calculated
		mHUDHeart->mAnimationDirection = HeartAnimationDirection;
		//update heart position and animation
		mHUDHeart->mPosition.x = heartPos.x; mHUDHeart->mPosition.y = heartPos.y;
		mHUDHeart->Update(dt);

		//health bar ------------------------------------------------------------------------------
		XMFLOAT2 deanHealthBarPos;
		//set health bar 50px underneath heart
		deanHealthBarPos.x = heartPos.x;
		deanHealthBarPos.y = heartPos.y - 50.0f;
		//update position of health bar BG
		mDeanyHealthBG->mPosition.x = deanHealthBarPos.x;
		mDeanyHealthBG->mPosition.y = deanHealthBarPos.y;
		mDeanyHealthBG->Update(dt);
		//calculate % of health from max
		float HealthPercent = ((float)mDeanyP->mHealth / mDeanyP->mMaxHealth);
		//calculate width difference based on % health of max (100.0f max width hard coded)
		float deanHealthWidthDelta = (100.0f - HealthPercent * 100.0f) / 2.0f;
		//update positions (x position to be updated relative to calculated width)
		mDeanyHealth->mPosition.x = deanHealthBarPos.x - deanHealthWidthDelta;
		mDeanyHealth->mPosition.y = deanHealthBarPos.y;
		//set scale and update
		mDeanyHealth->SetScale(HealthPercent, 1.0f);
		mDeanyHealth->Update(dt);

#pragma endregion HUDUpdates

		//flag game over if all lives lost
		if (mDeanyP->mLifeState == DPhoenix::DP_GAMEOVER_LIFESTATE)
			mGameState = GAME_OVER_STATE;
		//if all enemies defeated, set win state
		if (mAngelsCollected == mAngelsTotal)
			mGameState = GAME_WIN_STATE;
	}
	break;
	case GAME_WIN_STATE:
		//in here or the if statemnts above you could do things
		//like manage audio / animations specifically for this state
		break;

	case GAME_OVER_STATE:
		//in here or the if statemnts above you could do things
		//like manage audio / animations specifically for this state
		break;

	}

}


void EnemiesDemo::BuildGeometryBuffers()
{
#pragma region GeometryCreation


#pragma endregion GeometryCreation

#pragma region BlendStates
	//CREATE BLEND STATE ==========================================
	//this is so transparency can be layered for our sprites

	//create blend description struct and set properties
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable = TRUE;						//enable alpha blend
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;				//blend operation (add colour)
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;			//source blend - alpha 
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	//dest blend - inverse source alpha
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;		//blend operation for alpha
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;			//blend based fully on source
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;		//not blended based on destination
																		//this can affect the blend / transparency with different hex values
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//blend state (allow transparency)
	ID3D11BlendState* alphaBlendState_;
	//create the blend state with the properties set
	//OM stands for output merger - final stage of shader pipeline
	md3dDevice->CreateBlendState(&blendDesc, &alphaBlendState_);
	md3dImmediateContext->OMSetBlendState(alphaBlendState_, 0, 0xffffffff);
#pragma endregion BlendStates

#pragma region DepthStencilStates

	//DEPTH STENCIL STATES ==================================================
	//disabled depth stenciling will enable for 2D sprites to be layered on top of each other
	//we will use the standard depth stencilling for 3D rendering

	// Initialize the description of the stencil state.
	ZeroMemory(&mDepthStencilDesc, sizeof(mDepthStencilDesc));

	// Set up the description of the stencil state.
	//enable depth checking
	mDepthStencilDesc.DepthEnable = true;
	//use full buffer for depth checks
	mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//compare depth data
	//If the source data is less than the destination data, the comparison passes.
	mDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;


	//enable stencil testing
	mDepthStencilDesc.StencilEnable = true;
	//use all buffer for reading
	mDepthStencilDesc.StencilReadMask = 0xFF;
	//use all buffer for writing
	mDepthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	//keep if stencil test fails
	mDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//increment if stencil and depth fails
	mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//keep if stencil test passes
	mDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//always compare data gainst existing
	mDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	//keep if stencil test fails
	mDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//decrement if stencil and depth fails
	mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//keep if stencil test passes
	mDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//always compare data gainst existing
	mDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	HRESULT result = md3dDevice->CreateDepthStencilState(&mDepthStencilDesc, &mDepthStencilState);
	if (FAILED(result))
	{
		//like oh no scoob
		//return false;
	}

	// Set the depth stencil state.
	md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 1);

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&mDepthDisabledStencilDesc, sizeof(mDepthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	mDepthDisabledStencilDesc.DepthEnable = false;
	mDepthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	mDepthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mDepthDisabledStencilDesc.StencilEnable = true;
	mDepthDisabledStencilDesc.StencilReadMask = 0xFF;
	mDepthDisabledStencilDesc.StencilWriteMask = 0xFF;
	mDepthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	mDepthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	mDepthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	mDepthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = md3dDevice->CreateDepthStencilState(&mDepthDisabledStencilDesc, &mDepthDisabledStencilState);
	if (FAILED(result))
	{
		//return false;
	}

#pragma endregion DepthStencilStates

}

void EnemiesDemo::DrawScene()
{

#pragma region InitialRenderingSetup
	//do we have the context and swapchain?
	assert(md3dImmediateContext);
	assert(mSwapChain);

	//set the effect techniques we wish to use
	ID3DX11EffectTechnique* activeSpriteTech = DPhoenix::Effects::SpriteFX->SpriteTech;

	//clear the screen with the defined colour
	float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//set up the vertex input layout
	//(would need to change for different types of rendering)
	md3dImmediateContext->IASetInputLayout(DPhoenix::InputLayouts::SpritePosTex);
	//set primitive topology as triangle list (adjacent trinagles to render for GPU efficiency)
	md3dImmediateContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//clear all Direct2D before rendering any text
	//(this overwrites the 3D clear color)
	m2D1RT->BeginDraw();

	m2D1RT->Clear(D2D1::ColorF(0x00000000));

	m2D1RT->EndDraw();

#pragma endregion InitialRenderingSetup

#pragma region RenderSprites

	//to render sprites we need to disable depth testing (so it layers on top of each other)
	//like a collage
	md3dImmediateContext->OMSetDepthStencilState(mDepthDisabledStencilState, 1);

	switch (mGameState)
	{
	case GAME_TITLE_STATE:
	{
		RenderSprite(mTitleSprite, activeSpriteTech, true);
	}
	break;
	case GAME_PLAY_STATE:
	case GAME_WIN_STATE:
	case GAME_OVER_STATE:
	{
		//bg render first - other assets on top
		RenderSprite(mBGSprite, activeSpriteTech);

		//render map
		RenderMap(activeSpriteTech);

		//render collectables
		for (int i = 0; i < mCollectablesVec.size(); i++)
		{
			RenderSprite(mCollectablesVec[i]->mCollectibleSprite, activeSpriteTech);
		}

		//render player fire
		for (int i = 0; i < mPlayerFireballs.size(); i++)
		{
			RenderSprite(mPlayerFireballs[i]->mFireSprite, activeSpriteTech);
		}
		//enemy fireballs
		for (int i = 0; i < mEnemyFireballs.size(); i++)
		{
			RenderSprite(mEnemyFireballs[i]->mFireSprite, activeSpriteTech);
		}
		//enemies
		for (int i = 0; i < mEnemiesVec.size(); i++)
		{
			if (mEnemiesVec[i]->mIsActive)
				RenderSprite(mEnemiesVec[i]->mCurrentSprite, activeSpriteTech);
		}
		//angels
		for (int i = 0; i < mAngelsVec.size(); i++)
		{
			RenderSprite(mAngelsVec[i]->mAngelSprite, activeSpriteTech);
		}
		//player
		RenderSprite(mDeanyP->mCurrentSprite, activeSpriteTech);

#pragma region HUDRender

		//true param set as renders relative to screen not camera
		//background before foreground HUD elements

		RenderSprite(mDeanyHealthBG, activeSpriteTech, true);
		RenderSprite(mDeanyHealth, activeSpriteTech, true);

		RenderSprite(mHUDKnowlegdeLabel, activeSpriteTech, true);

		RenderSprite(mHUDBackCog, activeSpriteTech, true);
		RenderSprite(mHUDFrontCog, activeSpriteTech, true);

		RenderSprite(mHUDHeart, activeSpriteTech, true);

#pragma endregion HUDRender

		if(mGameState == GAME_WIN_STATE)
			RenderSprite(mWinSprite, activeSpriteTech, true);

		if (mGameState == GAME_OVER_STATE)
			RenderSprite(mGameOverSprite, activeSpriteTech, true);
	}
	break;
	}

	//turn depth stencil back on for 3D rendering
	md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 1);

#pragma endregion RenderSprites

#pragma region RenderFonts

	//string stream object for getting all text to display
	std::ostringstream debugText;

	debugText << "Debug text:" << std::endl;
	debugText << "Health: " << mDeanyP->mHealth <<
		" / " << mDeanyP->mMaxHealth << std::endl;
	debugText << "Knowledge: " << mDeanyP->mKnowledge <<
		" / " << mDeanyP->mMaxKnowledge << std::endl;
	debugText << "Enemies: " << mAngelsCollected <<
		" / " << mAngelsTotal << std::endl;
	debugText << "Lives: " << mDeanyP->mLives <<
		std::endl;

	//calculate screen scaling
	float scaleX = (float)mClientWidth / 1440;
	float scaleY = (float)mClientHeight / 900;

	//call draw font method sending all key info
	mDebugFont->DrawFont(m2D1RT, debugText.str(), 10.0f, 500.0f,
		1.0f, 1.0f, D2D1::ColorF(0xFFFFFF, 0.5f));

#pragma endregion RenderFonts

	//present to screen
	HR(mSwapChain->Present(0, 0));

}

void EnemiesDemo::HandleEvents(DPhoenix::IEvent* e)
{
	switch (e->GetID())
	{
	case DPhoenix::EVENT_KEYPRESS:
	{
		DPhoenix::KeyPressEvent* kpe = (DPhoenix::KeyPressEvent*)e;
		switch (kpe->mKeycode)
		{
		case DIK_D:
			mDeanyP->mInputs[(int)DPhoenix::DP_RIGHT_INPUT] = true;
			break;
		case DIK_A:
			mDeanyP->mInputs[(int)DPhoenix::DP_LEFT_INPUT] = true;
			break;
		case DIK_W:
			mDeanyP->mInputs[(int)DPhoenix::DP_UP_INPUT] = true;
			break;
		case DIK_S:
			mDeanyP->mInputs[(int)DPhoenix::DP_DOWN_INPUT] = true;
			break;
		case DIK_UP:
			mDeanyP->mInputs[(int)DPhoenix::DP_JUMP_INPUT] = true;
			break;
		case DIK_DOWN:
			mDeanyP->mInputs[(int)DPhoenix::DP_REFLECT_INPUT] = true;
			break;
		case DIK_SPACE:
			mDeanyP->mInputs[(int)DPhoenix::DP_FIRE_INPUT] = true;
			break;
		case DIK_J:
			mCamera->mInputs[(int)DPhoenix::CAMERA2D_LEFT_INPUT] = true;
			break;
		case DIK_L:
			mCamera->mInputs[(int)DPhoenix::CAMERA2D_RIGHT_INPUT] = true;
			break;
		case DIK_I:
			mCamera->mInputs[(int)DPhoenix::CAMERA2D_UP_INPUT] = true;
			break;
		case DIK_K:
			mCamera->mInputs[(int)DPhoenix::CAMERA2D_DOWN_INPUT] = true;
			break;
		case DIK_RETURN:
			mStartPressedFlag = true;
			if (mStartPressedFlag != mStartWasPressedFlag)
			{
				if (mGameState == GAME_TITLE_STATE)
				{
					mGameState = GAME_PLAY_STATE;
					mAudioMgr.GetSound("IgnoranceTitle")->Stop();
					mAudioMgr.GetSound("IgnoranceTitle")->SetPosition(0);
				}
				else if (mGameState == GAME_WIN_STATE ||
					mGameState == GAME_OVER_STATE)
				{
					ReInit();
				}
			}
			break;
		case DIK_MINUS:
			mDeanyP->mHealth -= 1;
			break;
		case DIK_EQUALS:
			mDeanyP->mHealth += 1;
			break;
		case DIK_ESCAPE:
			//shutdown here
			break;
		}
	}
	break;
	case DPhoenix::EVENT_XBOX_INPUT:
	{
		DPhoenix::XBoxEvent* xbe = (DPhoenix::XBoxEvent*)e;
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			mDeanyP->mInputs[(int)DPhoenix::DP_LEFT_INPUT] = true;
		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
		{
			mDeanyP->mInputs[(int)DPhoenix::DP_RIGHT_INPUT] = true;
		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
		{
			mDeanyP->mInputs[(int)DPhoenix::DP_UP_INPUT] = true;
		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		{
			mDeanyP->mInputs[(int)DPhoenix::DP_DOWN_INPUT] = true;
		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
		{
			mDeanyP->mInputs[(int)DPhoenix::DP_FIRE_INPUT] = true;
		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_B)
		{
			mDeanyP->mInputs[(int)DPhoenix::DP_JUMP_INPUT] = true;
		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_X)
		{
			mDeanyP->mInputs[(int)DPhoenix::DP_REFLECT_INPUT] = true;
		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
		{

		}
		if (xbe->mPadState.Gamepad.wButtons & XINPUT_GAMEPAD_START)
		{
			mStartPressedFlag = true;
			if (mStartPressedFlag != mStartWasPressedFlag)
			{
				if (mGameState == GAME_TITLE_STATE)
				{
					mGameState = GAME_PLAY_STATE;
					mAudioMgr.GetSound("IgnoranceTitle")->Stop();
					mAudioMgr.GetSound("IgnoranceTitle")->SetPosition(0);
				}
				else if (mGameState == GAME_WIN_STATE ||
					mGameState == GAME_OVER_STATE)
				{
					ReInit();
				}
			}
		}
	}
	break;
	}
}

void EnemiesDemo::InitAudio()
{
	mAudioMgr.CreateSound("IgnoranceTitle", "Audio\\Music\\IgnoranceTitle.wav");
	mAudioMgr.CreateSound("LaserShootFX", "Audio\\FX\\LaserShootFX.wav");
	mAudioMgr.CreateSound("FlatBelieveClip", "Audio\\Voice\\FlatBelieveClip.wav");

}

void EnemiesDemo::RenderSprite(DPhoenix::Sprite * sprite, ID3DX11EffectTechnique * tech, bool isHUD)
{
	//here we calculate the scale on X and Y based on the client width / height
	//vs. our original resolution 
	float scaleX = mScreenScale.x;
	float scaleY = mScreenScale.y;

	//add to a vector to use in matrices
	XMFLOAT3 scaleVec; scaleVec.x = scaleX;
	scaleVec.y = scaleY; scaleVec.z = 1.0f;

	//create view / projection matrix
	XMMATRIX viewProj;

	//HUD objects not affected by cameras
	if (!isHUD)
	{
		//here you would add any 2D camera control
		viewProj = mCamera->GetViewProj(sprite->mParallax);
	}
	else
	{
		//an identity matrix is standard and used as a basis
		//for other calculations
		XMMATRIX view = XMMatrixIdentity();
		//orthographic, left handed co-ordinates
		XMMATRIX projection = XMMatrixOrthographicLH(
			(float)mClientWidth,	//width of viewport
			(float)mClientHeight,	//height of viewport
			0.1f,					//nearplane (Z)
			100.0f					//far plane (Z)
		);

		//create view / projection matrix (multiply / concatenate)
		//these represent the 'camera'
		viewProj = XMMatrixMultiply(view, projection);
	}

	//concatenate world / view / projection matrices for rendering
	XMMATRIX world = sprite->CalculateTransforms(scaleVec, isHUD);
	XMMATRIX worldViewProj = XMMatrixMultiply(world, viewProj);
	DPhoenix::Effects::SpriteFX->SetWorldViewProj(worldViewProj);

	//now we get all the matching Sprite properties to send to the shader
	DPhoenix::Effects::SpriteFX->SetDiffuseMap(sprite->GetShaderResourceView());			//texture
	DPhoenix::Effects::SpriteFX->SetColumn(sprite->GetCurrentFrame());						//col / frame
	DPhoenix::Effects::SpriteFX->SetNumCols(sprite->mAnimationColumns);						//num frames
	DPhoenix::Effects::SpriteFX->SetFlipValue(sprite->mFlipValue);							//flip value
	DPhoenix::Effects::SpriteFX->SetLerpValue(sprite->mColorLerpValue);						//lerp value
	DPhoenix::Effects::SpriteFX->SetOpacityValue(sprite->mOpacityValue);					//opacity
	DPhoenix::Effects::SpriteFX->SetAltColorValue(XMLoadFloat4(&sprite->mAltColorValue));	//alt color

																							//render using effect shader technique
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		//use current context and get current rendering pass
		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//call Render method of Sprite object
		sprite->Render(md3dImmediateContext);
	}

	//reset the lerp value on the shader
	DPhoenix::Effects::SpriteFX->SetLerpValue(0.0f);
}

void EnemiesDemo::InitBackground()
{
	mBGSprite = new DPhoenix::Sprite();
	mBGSprite->Load("Textures\\LevelAssets\\BGdiv8.png", &mTexMgr, 2340.0f, 1125.0f, md3dDevice);
	mBGSprite->SetCurrentFrame(0);	mBGSprite->mAnimationColumns = 1;
	mBGSprite->mAnimationDirection = 0;	mBGSprite->SetAnimationRange(0, 0);
	mBGSprite->SetScale(1.0f);	mBGSprite->mFlipValue = 1.0f;

	//parallax value (pre-calculated)
	//advanced technique is to calculate it based
	//on how you want it to scroll (involves logarithmic functions)
	mBGSprite->mParallax.x = 0.125f; 	mBGSprite->mParallax.y = 0.125f;
}

void EnemiesDemo::RenderMap(ID3DX11EffectTechnique* tech)
{
	//calculate any scaling values if screen stretched
	float scaleX = (float)mClientWidth / 1440;
	float scaleY = (float)mClientHeight / 900;
	//adjust to XMFLOAT3
	XMFLOAT3 scaleVec; scaleVec.x = scaleX;
	scaleVec.y = scaleY; scaleVec.z = 1.0f;
	//get view*projection matrix from camera
	XMMATRIX viewProj = mCamera->GetViewProj(XMFLOAT2(1.0f, 1.0f));

	//calculate where to begin and end array checking based on camera position and screen dimensions
	int colBegin = mMap->GetMapTileYAtPoint(mCamera->mPosition.y - mClientHeight / 2);
	int colEnd = mMap->GetMapTileYAtPoint(mCamera->mPosition.y + mClientHeight / 2) + 1;

	int rowBegin = mMap->GetMapTileXAtPoint(mCamera->mPosition.x - mClientWidth / 2);
	int rowEnd = mMap->GetMapTileXAtPoint(mCamera->mPosition.x + mClientWidth / 2) + 1;

	//bound the array values within the map array indices
	if (rowBegin < 0) rowBegin = 0; if (colBegin < 0) colBegin = 0;
	if (rowEnd > mMap->mWidth) rowEnd = mMap->mWidth;
	if (colEnd > mMap->mHeight) colEnd = mMap->mHeight;

	//loop through the map tile array (rows then columns) based on boundings
	//calculated above
	for (int row = rowBegin; row < rowEnd; row++)
	{
		for (int col = colBegin; col < colEnd; col++)
		{
			//only render if not empty tiles
			if (mMap->mTiles[row][col] != DPhoenix::EMPTY_TILE)
			{
				//check tile type to load correct texture
				if (mMap->mTiles[row][col] == DPhoenix::BLOCK_TILE)
				{
					DPhoenix::Effects::SpriteFX->SetDiffuseMap(
						mMap->mSolidTileSprite->GetShaderResourceView());
				}
				else if (mMap->mTiles[row][col] == DPhoenix::ONEWAY_TILE)
				{
					DPhoenix::Effects::SpriteFX->SetDiffuseMap(
						mMap->mOneWayTileSprite->GetShaderResourceView());
				}
				//set the tile position 
				XMFLOAT2 tilePos = mMap->GetMapTilePosition(row, col);
				mMap->mSolidTileSprite->mPosition.x = tilePos.x;
				mMap->mSolidTileSprite->mPosition.y = tilePos.y;
				//calculate and set matrices
				XMMATRIX world = mMap->mSolidTileSprite->CalculateTransforms(scaleVec);
				XMMATRIX worldViewProj = XMMatrixMultiply(world, viewProj);
				DPhoenix::Effects::SpriteFX->SetWorldViewProj(worldViewProj);
				//set standard shader values (use solid sprite as no difference in these values)
				DPhoenix::Effects::SpriteFX->SetColumn(mMap->mSolidTileSprite->GetCurrentFrame());
				DPhoenix::Effects::SpriteFX->SetNumCols(mMap->mSolidTileSprite->mAnimationColumns);
				DPhoenix::Effects::SpriteFX->SetFlipValue(mMap->mSolidTileSprite->mFlipValue);
				//use technique to render
				D3DX11_TECHNIQUE_DESC techDesc;
				tech->GetDesc(&techDesc);
				for (UINT p = 0; p < techDesc.Passes; ++p)
				{
					tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
					mMap->mSolidTileSprite->Render(md3dImmediateContext);
				}
			}
		}
	}
}
//re-initialise key game assets for replay
void EnemiesDemo::ReInit()
{

#pragma region MainGameplayInit

	//player - reset stats and states
	mDeanyP->mLives = 3; mDeanyP->mMaxKnowledge = 100; mDeanyP->mKnowledge = 100;
	mDeanyP->mMaxHealth = 100; mDeanyP->mHealth = 100;
	mDeanyP->mLifeState = DPhoenix::DP_OK_LIFESTATE;
	mDeanyP->mLastLifeFlag = false;
	//clear existing vectors
	mCollectablesVec.clear(); mEnemySpawnVec.clear();
	mEnemiesVec.clear();
	
	//map - reload
	mMap = new DPhoenix::Map("Data\\Levels\\TestLevel.csv", &mTexMgr, md3dDevice, 
		mCollectablesVec, mEnemySpawnVec,
		48, 30, 2, 1);
	//reset player position to spawn point
	mDeanyP->mPosition = mMap->mPlayerSpawnPoint;
	//clear the fireball vectors
	mEnemyFireballs.clear();
	mPlayerFireballs.clear();
	
	//enemies loading (as in Init)
	for (int i = 0; i < mEnemySpawnVec.size(); i++)
	{

		switch ((DPhoenix::EnemyTypesForMap)i)
		{
		case DPhoenix::ENEMYMAP_CHEMPLANE:
			for (int j = 0; j < mEnemySpawnVec[i].size(); j++)
			{
				mEnemiesVec.push_back(new DPhoenix::ChemPlane(&mTexMgr, md3dDevice, 
					*mEnemySpawnVec[i][j], &mAudioMgr));
			}

			break;
		case DPhoenix::ENEMYMAP_ANTIVAX:
			for (int j = 0; j < mEnemySpawnVec[i].size(); j++)
			{
				mEnemiesVec.push_back(new DPhoenix::AntiVax(&mTexMgr, md3dDevice, 
					*mEnemySpawnVec[i][j], &mAudioMgr));
			}

			break;
		}
	}
	//set angels max and collected
	mAngelsCollected = 0;
	mAngelsTotal = mEnemiesVec.size();
	//camera position reset  (as in Init)
	mCamera->mPosition.x = 0.0f;  mCamera->mPosition.y = 0.0f;
	mCamera->mSpeed.x = 0.0f;  mCamera->mSpeed.y = 0.0f;

	float tileAdjust = mMap->mTileSize / 2;
	mCamera->mLeftLimit = (mMap->mPosition.x + mClientWidth / 2) - tileAdjust;
	mCamera->mRightLimit = mCamera->mLeftLimit + mMap->mWidth * mMap->mTileSize - mClientWidth;
	mCamera->mBottomLimit = (mMap->mPosition.y + mClientHeight / 2) - tileAdjust;
	mCamera->mTopLimit = mCamera->mBottomLimit + mMap->mHeight * mMap->mTileSize - mClientHeight;

	//HUD ------------------------------------------------------------------------
	mHUDKnowlegdeLabel->SetCurrentFrame(0);

	mCogRotationValue = 0.0f;
	mHUDFrontCogLerpValue = 0.0f;

	mHUDHeart->SetCurrentFrame(0);
	mHUDHeart->mAnimationDirection = 10;

	mHUDHeartLerpValue = 0.0f;

	mDeanyHealthBG->SetCurrentFrame(0);
	mDeanyHealthBG->SetScale(1.0f);
	mDeanyHealthBG->mAltColorValue = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);

	mDeanyHealth->SetCurrentFrame(0);
	mDeanyHealth->SetScale(1.0f);
	mDeanyHealth->mAltColorValue = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);

	//end HUD --------------------------------------------------------------------
			
#pragma endregion MainGameplayInit

	//reset all sounds and begin title state
	mAudioMgr.ResetAllSounds();
	mAudioMgr.GetSound("IgnoranceTitle")->Play(true);
	mGameState = GAME_TITLE_STATE;
}

