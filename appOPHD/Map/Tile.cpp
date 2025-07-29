#include "Tile.h"

#include "../MapObjects/Robot.h"
#include "../MapObjects/Structure.h"

#include <libOPHD/EnumTerrainType.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <stdexcept>


Tile::Tile() :
	mIndex{TerrainType::Dozed}
{
}


Tile::Tile(const MapCoordinate& position, TerrainType index) :
	mIndex{index},
	mPosition{position}
{}


Tile::Tile(Tile&& other) noexcept :
	mIndex{other.mIndex},
	mPosition{other.mPosition},
	mMapObject{other.mMapObject},
	mOreDeposit{other.mOreDeposit},
	mOverlay{other.mOverlay},
	mExcavated{other.mExcavated}
{
	other.mMapObject = nullptr;
	other.mOreDeposit = nullptr;
}


Tile& Tile::operator=(Tile&& other) noexcept
{
	mIndex = other.mIndex;
	mPosition = other.mPosition;
	mMapObject = other.mMapObject;
	mOreDeposit = other.mOreDeposit;
	mOverlay = other.mOverlay;
	mExcavated = other.mExcavated;

	other.mMapObject = nullptr;
	other.mOreDeposit = nullptr;

	return *this;
}


Tile::~Tile()
{
	delete mOreDeposit;
	delete mMapObject;
}


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


/**
 * Adds a new MapObject to the tile.
 *
 * \param	mapObject		Pointer to a MapObject.
 */
void Tile::mapObject(MapObject* mapObject)
{
	if (mMapObject)
	{
		throw std::runtime_error("Attempting to set MapObject on a tile where it's already set");
	}

	mMapObject = mapObject;
}


/**
 * Clears a MapObject from the Tile.
 *
 * \note	Garbage collects the MapObject. Deletes and frees memory.
 */
void Tile::deleteMapObject()
{
	delete mMapObject;
	removeMapObject();
}


/**
 * Removes a MapObject from the Tile.
 *
 * \note	Does NOT delete or free memory. Simply clears the pointer, not the memory.
 */
void Tile::removeMapObject()
{
	mMapObject = nullptr;
}


void Tile::placeOreDeposit(OreDeposit* oreDeposit)
{
	delete mOreDeposit;
	mOreDeposit = oreDeposit;
}


Structure* Tile::structure() const
{
	return dynamic_cast<Structure*>(mapObject());
}


Robot* Tile::robot() const
{
	return dynamic_cast<Robot*>(mapObject());
}
