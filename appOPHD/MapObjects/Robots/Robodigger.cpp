#include "Robodigger.h"

#include "../../Constants/Strings.h"

#include <libOPHD/EnumDirection.h>

#include <NAS2D/Dictionary.h>


Robodigger::Robodigger() :
	Robot(RobotTypeIndex::Digger),
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


void Robodigger::onTaskComplete(TileMap& /*tileMap*/, StructureManager& /*structureManager*/)
{
}
