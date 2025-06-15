#include "MapObject.h"


MapObject::MapObject(const std::string& name, const std::string& spritePath, const std::string& initialAction) :
	mName(name),
	mSprite(spritePath, initialAction)
{}


const std::string& MapObject::name() const
{
	return mName;
}


NAS2D::Sprite& MapObject::sprite()
{
	return mSprite;
}


void MapObject::die()
{
	mIsDead = true;
}


/// MapObject is dead and should be cleaned up.
bool MapObject::isDead() const
{
	return mIsDead;
}
