#include "MapObject.h"

#include <NAS2D/Math/Point.h>


MapObject::MapObject(const std::string& spritePath, const std::string& initialAction) :
	mSprite(spritePath, initialAction)
{}


void MapObject::updateAnimation()
{
	mSprite.update();
}


void MapObject::draw(NAS2D::Point<int> position) const
{
	mSprite.draw(position);
}
