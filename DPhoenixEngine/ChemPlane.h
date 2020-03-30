#pragma once
#include "Enemy.h"

namespace DPhoenix
{
	enum ChemPlaneMoveStates
	{
		CHEM_HOVER_MOVESTATE,
		CHEM_ZOOM_MOVESTATE
	};

	class ChemPlane : public Enemy
	{
	public:
		//move state (either hovers briefly or zooms quickly)
		ChemPlaneMoveStates mMoveState;
		//spritesheet (only one for this enemy)
		Sprite* mChemPlaneSprite;

		//timer for movement
		GameTimer mMoveTimer;
		//(you might have other timers, such as for voice clips)
		
		//constructor, takes position and device pointers
		ChemPlane(TextureMgr* mTexMgr, ID3D11Device* md3dDevice,
			XMFLOAT2 position, AudioMgr* audioMgr);
		~ChemPlane() { };

		//update logic
		void EnemyUpdate(float deltaTime, DPhoenix::Map* map, XMFLOAT2 playerPos);
		//handle damage
		bool TakeDamage(int dmg, AABB2D fireBox);
		//reset timers
		void ResetTimers();
	};
}

