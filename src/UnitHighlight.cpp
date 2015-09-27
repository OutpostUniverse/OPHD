#include "UnitHighlight.h"


#include <math.h>

const float			DEFAULT_BOUNCE_SPEED	=	75.0f;
const float			DEFAULT_TRAVEL_DISTANCE	=	4.5f;
const std::string	UNIT_HIGHLIGHT_SPRITE	=	"ui/unit_select.xml";



UnitHighlight::UnitHighlight():		mBounceSpeed(DEFAULT_BOUNCE_SPEED),
									mTravelDistance(DEFAULT_TRAVEL_DISTANCE),
									mHighlightSprite(UNIT_HIGHLIGHT_SPRITE),
									mBounce(true)
{
	mHighlightSprite.play("throb");
}


UnitHighlight::~UnitHighlight()
{
}


void UnitHighlight::draw(float x, float y)
{
	if(mBounce)
		mHighlightSprite.update(x, y + sin(mTimer.tick() / mBounceSpeed) * DEFAULT_TRAVEL_DISTANCE);
	else
		mHighlightSprite.update(x, y);
}
