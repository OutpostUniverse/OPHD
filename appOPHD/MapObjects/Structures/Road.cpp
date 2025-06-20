#include "Road.h"

#include "../../Map/Connections.h"


void Road::updateConnections(const TileMap& tileMap)
{
	if (operational())
	{
		mSprite.play(roadAnimationName(*this, tileMap));
	}
}
