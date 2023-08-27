#include "Tile.h"

#include "../MapObjects/Mine.h"
#include "../MapObjects/Robot.h"
#include "../MapObjects/Structure.h"

#include "../Constants/Numbers.h"

#include <cfloat>


Tile::Tile(const MapCoordinate& position, TerrainType index) :
	mIndex{index},
	mPosition{position}
{}


Tile::Tile(Tile&& other) noexcept :
	mIndex{other.mIndex},
	mPosition{other.mPosition},
	mMapObject{other.mMapObject},
	mMine{other.mMine},
	mOverlay{other.mOverlay},
	mExcavated{other.mExcavated}
{
	other.mMapObject = nullptr;
	other.mMine = nullptr;
}


Tile& Tile::operator=(Tile&& other) noexcept
{
	mIndex = other.mIndex;
	mPosition = other.mPosition;
	mMapObject = other.mMapObject;
	mMine = other.mMine;
	mOverlay = other.mOverlay;
	mExcavated = other.mExcavated;

	other.mMapObject = nullptr;
	other.mMine = nullptr;

	return *this;
}


Tile::~Tile()
{
	delete mMine;
	delete mMapObject;
}


/**
 * Adds a new MapObject to the tile.
 *
 * \param	mapObject		Pointer to a MapObject.
 */
void Tile::pushMapObject(MapObject* mapObject)
{
	if (mMapObject)
	{
		if (mMapObject == mapObject)
		{
			throw std::runtime_error("Attempting to pushMapObject on a tile where it's already set");
		}
		deleteMapObject();
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


void Tile::pushMine(Mine* mine)
{
	delete mMine;
	mMine = mine;
}


Structure* Tile::structure() const
{
	return dynamic_cast<Structure*>(thing());
}


Robot* Tile::robot() const
{
	return dynamic_cast<Robot*>(thing());
}


float Tile::movementCost() const
{
	if (index() == TerrainType::Impassable)
	{
		return FLT_MAX;
	}

	if (!empty() && thingIsStructure() && structure()->isRoad())
	{
		Structure& road = *structure();

		if (road.state() != StructureState::Operational)
		{
			return constants::RouteBaseCost * static_cast<float>(TerrainType::Difficult) + 1.0f;
		}
		else if (road.integrity() < constants::RoadIntegrityChange)
		{
			return 0.75f;
		}
		else
		{
			return 0.5f;
		}
	}

	if (!empty() && (!thingIsStructure() || (!structure()->isMineFacility() && !structure()->isSmelter())))
	{
		return FLT_MAX;
	}

	return constants::RouteBaseCost * static_cast<float>(index()) + 1.0f;
}
