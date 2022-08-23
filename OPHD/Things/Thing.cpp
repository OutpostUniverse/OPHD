#include "Thing.h"


Thing::Thing(const std::string& name, const std::string& spritePath, const std::string& initialAction) :
	mName(name),
	mSprite(spritePath, initialAction)
{}


const std::string& Thing::name() const
{
	return mName;
}


NAS2D::Sprite& Thing::sprite()
{
	return mSprite;
}


void Thing::die()
{
	mIsDead = true;
	mDieSignal(this);
}


/// Thing is dead and should be cleaned up.
bool Thing::dead() const
{
	return mIsDead;
}


Thing::DieSignal::Source& Thing::onDie()
{
	return mDieSignal;
}
