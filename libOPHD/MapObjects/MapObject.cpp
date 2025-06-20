#include "MapObject.h"


MapObject::MapObject(const std::string& spritePath, const std::string& initialAction) :
	mSprite(spritePath, initialAction)
{}


NAS2D::Sprite& MapObject::sprite()
{
	return mSprite;
}
