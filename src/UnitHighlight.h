#ifndef __UNIT_HIGHLIGHTER__
#define __UNIT_HIGHLIGHTER__

#include "NAS2D/NAS2D.h"

using namespace NAS2D;

class UnitHighlight
{
public:
	UnitHighlight();
	~UnitHighlight();

	void bounce(bool _b) { mBounce = _b; }
	bool bounce() const { return mBounce; }

	void bounceSpeed(float speed) { mBounceSpeed = speed; }
	float bounceSpeed() const { return mBounceSpeed; }

	void draw(float x, float y);

private:
	
	float		mBounceSpeed;
	float		mTravelDistance;
	
	Timer		mTimer;

	Sprite		mHighlightSprite;

	bool		mBounce;
};

#endif