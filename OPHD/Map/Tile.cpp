#include "Tile.h"

#include "../Mine.h"
#include "../Things/Robots/Robot.h"
#include "../Things/Structures/Structure.h"

#include "../Constants/Numbers.h"

#include <cfloat>


Tile::Tile(const MapCoordinate& position, TerrainType index) :
	mIndex{index},
	mPosition{position}
{}


Tile::Tile(Tile&& other) noexcept :
	mIndex{other.mIndex},
	mPosition{other.mPosition},
	mThing{other.mThing},
	mMine{other.mMine},
	mOverlay{other.mOverlay},
	mExcavated{other.mExcavated}
{
	other.mThing = nullptr;
	other.mMine = nullptr;
}


Tile& Tile::operator=(Tile&& other) noexcept
{
	mIndex = other.mIndex;
	mPosition = other.mPosition;
	mThing = other.mThing;
	mMine = other.mMine;
	mOverlay = other.mOverlay;
	mExcavated = other.mExcavated;

	other.mThing = nullptr;
	other.mMine = nullptr;

	return *this;
}


Tile::~Tile()
{
	delete mMine;
	delete mThing;
}


/**
 * Adds a new Thing to the tile.
 *
 * \param	thing		Pointer to a Thing.
 */
void Tile::pushThing(Thing* thing)
{
	if (mThing)
	{
		if (mThing == thing)
		{
			throw std::runtime_error("Attempting to pushThing on a tile where it's already set");
		}
		deleteThing();
	}

	mThing = thing;
}


/**
 * Clears a Thing from the Tile.
 *
 * \note	Garbage collects the Thing. Deletes and frees memory.
 */
void Tile::deleteThing()
{
	delete mThing;
	removeThing();
}


/**
 * Removes a Thing from the Tile.
 *
 * \note	Does NOT delete or free memory. Simply clears the pointer, not the memory.
 */
void Tile::removeThing()
{
	mThing = nullptr;
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
	const Tile& tile = *this;

	if (tile.index() == TerrainType::Impassable)
	{
		return FLT_MAX;
	}

	if (!tile.empty() && tile.thingIsStructure() && tile.structure()->isRoad())
	{
		Structure& road = *tile.structure();

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

	if (!tile.empty() && (!tile.thingIsStructure() || (!tile.structure()->isMineFacility() && !tile.structure()->isSmelter())))
	{
		return FLT_MAX;
	}

	return constants::RouteBaseCost * static_cast<float>(tile.index()) + 1.0f;
}
