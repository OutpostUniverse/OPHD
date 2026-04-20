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


void Robodigger::target(const MapCoordinate& target)
{
	mTarget = target;
}


const MapCoordinate& Robodigger::target() const
{
	return mTarget;
}


NAS2D::Dictionary Robodigger::getDataDict() const
{
	auto dictionary = Robot::getDataDict();
	dictionary.set("direction", static_cast<int>(mDirection));
	dictionary.set("targetX", mTarget.xy.x);
	dictionary.set("targetY", mTarget.xy.y);
	dictionary.set("targetDepth", mTarget.z);
	return dictionary;
}


void Robodigger::onTaskComplete(TileMap& /*tileMap*/, StructureManager& /*structureManager*/)
{
}
