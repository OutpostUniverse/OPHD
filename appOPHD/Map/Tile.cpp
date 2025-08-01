#include "Tile.h"

#include "../MapObjects/Robot.h"
#include "../MapObjects/Structure.h"

#include <libOPHD/EnumTerrainType.h>

#include <stdexcept>


Tile::Tile() :
	mIndex{TerrainType::Dozed}
{
}


Tile::Tile(const MapCoordinate& position, TerrainType index) :
	mIndex{index},
	mPosition{position}
{}


bool Tile::isSurface() const
{
	return mPosition.z == 0;
}


bool Tile::isBulldozed() const
{
	return index() == TerrainType::Dozed;
}


void Tile::bulldoze()
{
	index(TerrainType::Dozed);
}


bool Tile::isImpassable() const
{
	return index() == TerrainType::Impassable;
}


void Tile::mapObject(MapObject* mapObject)
{
	if (mMapObject)
	{
		throw std::runtime_error("Attempting to set MapObject on a tile where it's already set");
	}

	mMapObject = mapObject;
}


void Tile::removeMapObject()
{
	mMapObject = nullptr;
}


void Tile::placeOreDeposit(OreDeposit* oreDeposit)
{
	if (mOreDeposit)
	{
		throw std::runtime_error("Attempting to set OreDeposit on a tile where it's already set");
	}
	mOreDeposit = oreDeposit;
}


void Tile::removeOreDeposit()
{
	mOreDeposit = nullptr;
}


Structure* Tile::structure() const
{
	return dynamic_cast<Structure*>(mapObject());
}


Robot* Tile::robot() const
{
	return dynamic_cast<Robot*>(mapObject());
}
