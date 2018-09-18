#pragma once

#include "../Common.h"

#include "../Mine.h"

#include "../Things/Thing.h"
#include "../Things/Structures/Structure.h"
#include "../Things/Robots/Robot.h"

class Tile
{
public:
	Tile();
	~Tile();

	int index() const { return mIndex; }
	void index(int index) { mIndex = index; }

	int x() const { return mX; }
	void x(int i) { mX = i; }

	int y() const { return mY; }
	void y(int i) { mY = i; }

	int depth() const { return mDepth; }
	void depth(int i) { mDepth = i; }

	void init(int x, int y, int depth, int index);

	bool bulldozed() const { return index() == 0; }

	bool excavated() const { return mExcavated; }
	void excavated(bool _b) { mExcavated = _b; }

	bool connected() const { return mConnected; }
	void connected(bool _b) { mConnected = _b; }

	Thing* thing() { return mThing; }

	bool empty() const { return mThing == nullptr; }

	bool hasMine() const { return mMine != nullptr; }

	Structure*	structure();
	Robot* robot();

	bool thingIsStructure() const { return mThingIsStructure; }

	void pushThing(Thing* thing, bool overwrite = true);
	void deleteThing();

	void removeThing();

	Mine* mine() { return mMine; }
	void pushMine(Mine* _mine);

	float distanceTo(Tile* _t);


protected:
	friend class StructureManager;

	// Access to this function should be very, very limited.
	void thingIsStructure(bool _b) { mThingIsStructure = _b; }

private:
	int				mIndex = 0;					/**<  */

	int				mX = 0;						/**< Tile Position Information */
	int				mY = 0;						/**< Tile Position Information */
	int				mDepth = 0;					/**< Tile Position Information */


	Thing*			mThing = nullptr;			/**<  */

	Mine*			mMine = nullptr;			/**<  */

	bool			mExcavated = true;			/**< Used when a Digger uncovers underground tiles. */
	bool			mConnected = false;			/**< Flag indicating that this tile is connected to the Command Center. */
	bool			mThingIsStructure = false;	/**< Flag indicating that the Thing in the tile is a Structure. */
};
