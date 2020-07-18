// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Tile.h"

#include <cmath>


Tile::Tile(NAS2D::Point<int> position, int depth, int index) :
	mIndex{index},
	mPosition{position},
	mDepth{depth}
{}


Tile::Tile(Tile&& other) noexcept :
	mIndex{other.mIndex},
	mPosition{other.mPosition},
	mDepth{other.mDepth},
	mThing{other.mThing},
	mMine{other.mMine},
	mColor{other.mColor},
	mExcavated{other.mExcavated},
	mThingIsStructure{other.mThingIsStructure}
{
	other.mThing = nullptr;
	other.mMine = nullptr;
}


Tile& Tile::operator=(Tile&& other) noexcept
{
	mIndex = other.mIndex;
	mPosition = other.mPosition;
	mDepth = other.mDepth;
	mThing = other.mThing;
	mMine = other.mMine;
	mColor = other.mColor;
	mExcavated = other.mExcavated;
	mThingIsStructure = other.mThingIsStructure;

	other.mThing = nullptr;
	other.mMine = nullptr;

	return *this;
}


/**
 * D'tor
 */
Tile::~Tile()
{
	delete mMine;
	delete mThing;
}


/**
 * Adds a new Thing to the tile.
 *
 * \param	thing		Pointer to a Thing.
 * \param	overwrite	Overwrite any existing Thing's that may already be in the Tile.
 */
void Tile::pushThing(Thing* thing)
{
	if (mThing)
	{
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
	thingIsStructure(false); // Cover all bases.
}


/**
 * 
 */
void Tile::pushMine(Mine* _mine)
{
	delete mMine;
	mMine = _mine;
}


/**
 * 
 */
Structure* Tile::structure()
{
	if (mThingIsStructure) { return static_cast<Structure*>(thing()); }

	return nullptr;
}


/**
 * 
 */
Robot* Tile::robot()
{
	// Assumption: Things in a tile can only be a Robot or a Structure. If the thing is not a
	// structure, it can only be a robot.
	if (!empty() && structure() == nullptr) { return static_cast<Robot*>(thing()); }

	return nullptr;
}


/**
 * 
 */
float Tile::distanceTo(Tile* tile)
{
	return distanceTo(tile->position());
}


float Tile::distanceTo(NAS2D::Point<int> point)
{
	const auto direction = point - position();
	return static_cast<float>(std::sqrt((direction.x * direction.x) + (direction.y * direction.y)));
}
