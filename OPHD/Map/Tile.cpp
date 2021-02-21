#include "Tile.h"

#include "../Mine.h"
#include "../Things/Robots/Robot.h"
#include "../Things/Structures/Structure.h"

#include <cmath>


Tile::Tile(NAS2D::Point<int> position, int depth, TerrainType index) :
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
	mExcavated{other.mExcavated}
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


void Tile::pushMine(Mine* _mine)
{
	delete mMine;
	mMine = _mine;
}


Structure* Tile::structure() const
{
	return dynamic_cast<Structure*>(thing());
}


Robot* Tile::robot() const
{
	return dynamic_cast<Robot*>(thing());
}


float Tile::distanceTo(Tile* tile)
{
	return distanceTo(tile->position());
}


float Tile::distanceTo(NAS2D::Point<int> point)
{
	const auto direction = point - position();
	return static_cast<float>(std::sqrt((direction.x * direction.x) + (direction.y * direction.y)));
}
