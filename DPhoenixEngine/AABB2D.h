#pragma once
#include "D3DUtil.h"

struct AABB2D
{
public:
	//properties for setting up box
	XMFLOAT2 center;
	XMFLOAT2 halfSize;

	//initialise - constructor
	AABB2D()
	{
		center.x = 0.0f;
		center.y = 0.0f;

		halfSize.x = 0.0f;
		halfSize.y = 0.0f;
	}
	//constructor that takes in values
	AABB2D(XMFLOAT2 _center, XMFLOAT2 _halfSize)
	{
		center = _center;
		halfSize = _halfSize;
	}
	//AABB collision test
	bool Overlaps(AABB2D other)
	{
		//is distance between centres greater than
		//addition of both x halfsizes?
		if (abs(center.x - other.center.x) >
			halfSize.x + other.halfSize.x) return false;

		//is distance between centres greater than
		//addition of both y halfsizes?
		if (abs(center.y - other.center.y) >
			halfSize.y + other.halfSize.y) return false;

		return true;
	}
};

