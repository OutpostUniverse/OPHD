#include "Tile.h"

Tile::Tile() :	mIndex(0),
				mThing(nullptr),
				mMine(nullptr),
				mExcavated(true),
				mConnected(false),
				mThingIsStructure(false)
{}

Tile::Tile(int idx) :	mIndex(idx),
						mThing(nullptr),
						mMine(nullptr),
						mExcavated(false),
						mConnected(false),
						mThingIsStructure(false)
{}

Tile::~Tile()
{
	if (mMine)
	{
		delete mMine;
		mMine = nullptr;
	}

	if (mThing)
	{
		delete mThing;
		mThing = nullptr;
	}
}

/**
* Adds a new Thing to the tile.
*
* \param	thing		Pointer to a Thing.
* \param	overwrite	Overwrite any existing Thing's that may already be in the Tile.
*/
void Tile::pushThing(Thing* thing, bool overwrite)
{
	if (mThing)
	{
		if (overwrite)
		{
			deleteThing();
		}
		else
		{
			// Clean up the thing passed into this function
			// as they're not references but newly created objects.
			delete thing;
			return;
		}
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
	thingIsStructure(false);	// Cover all bases.
}


void Tile::pushMine(Mine* _mine)
{
	if (mMine)
	{
		delete mMine;
		mMine = NULL;
	}

	mMine = _mine;
}


Structure* Tile::structure()
{
	if (mThingIsStructure)
		return static_cast<Structure*>(thing());
	
	return nullptr;
}


Robot* Tile::robot()
{
	if(!empty() && structure() == nullptr)
		return static_cast<Robot*>(thing());

	return nullptr;
}