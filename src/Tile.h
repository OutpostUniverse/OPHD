#ifndef __TILE__
#define __TILE__

#include "Things/Thing.h"
#include "Mine.h"
#include "Common.h"


class Tile
{
public:

	Tile();
	Tile(int idx);

	~Tile();

	int index() const { return mIndex; }
	void index(int index) { mIndex = index; }

	bool bulldozed() const { return mIndex == 0; }

	bool excavated() const { return mExcavated; }
	void excavated(bool _b) { mExcavated = _b; }

	bool connected() const { return mConnected; }
	void connected(bool _b) { mConnected = _b; }

	Thing* thing() { return mThing; }

	bool thingIsStructure() const { return mThingIsStructure; }

	void pushThing(Thing* thing, bool overwrite = true);
	void deleteThing();

	void removeThing();

	Mine* mine() { return mMine; }
	void pushMine(Mine* _mine);

protected:
	friend class StructureManager;

	// Access to this function should be very, very limited.
	void thingIsStructure(bool _b) { mThingIsStructure = _b; }


private:

	int				mIndex;
	Thing*			mThing;

	Mine*			mMine;

	bool			mExcavated;			/**< Used when a Digger uncovers underground tiles. */
	bool			mConnected;			/**< Flag indicating that this tile is connected to the Command Center. */
	bool			mThingIsStructure;	/**< Flag indicating that the Thing in the tile is a Structure. */
};

#endif