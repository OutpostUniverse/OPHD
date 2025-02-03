#include "Robodigger.h"

#include "../../Constants/Strings.h"

#include <libOPHD/EnumDirection.h>


Robodigger::Robodigger() :
	Robot(constants::Robodigger, "robots/robodigger.sprite", Robot::Type::Digger),
	mDirection(Direction::Down)
{
}


void Robodigger::direction(Direction dir)
{
	mDirection = dir;
}


Direction Robodigger::direction() const
{
	return mDirection;
}


NAS2D::Dictionary Robodigger::getDataDict() const
{
	auto dictionary = Robot::getDataDict();
	dictionary.set("direction", static_cast<int>(mDirection));
	return dictionary;
}
