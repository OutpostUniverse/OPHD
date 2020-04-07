#pragma once

#include "../Common.h"

#include "../Mine.h"

#include "../Things/Thing.h"
#include "../Things/Structures/Structure.h"
#include "../Things/Robots/Robot.h"

#include "NAS2D/Renderer/Point.h"
#include "NAS2D/Renderer/Vector.h"


class Tile
{
public:
	Tile() = default;
	Tile(Tile&& other);
	Tile& operator=(Tile&& other);
	~Tile();

	int index() const { return mIndex; }
	void index(int index) { mIndex = index; }

	int x() const { return mX; }
	void x(int i) { mX = i; }

	int y() const { return mY; }
	void y(int i) { mY = i; }

	NAS2D::Point<int> position() const { return {mX, mY}; }

	int depth() const { return mDepth; }
	void depth(int i) { mDepth = i; }

	void init(int x, int y, int depth, int index);

	bool bulldozed() const { return index() == 0; }

	bool excavated() const { return mExcavated; }
	void excavated(bool value) { mExcavated = value; }

	bool connected() const { return mConnected; }
	void connected(bool value) { mConnected = value; }

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
	void pushMine(Mine*);

	float distanceTo(Tile* tile);
	float distanceTo(NAS2D::Point<int> point);

	void color(const NAS2D::Color& c) { mColor = c; }
	const NAS2D::Color& color() const { return mColor; }

protected:
	friend class StructureManager;

	// Access to this function should be very, very limited.
	void thingIsStructure(bool value) { mThingIsStructure = value; }

private:
	int				mIndex = 0;

	int				mX = 0;						/**< Tile Position Information */
	int				mY = 0;						/**< Tile Position Information */
	int				mDepth = 0;					/**< Tile Position Information */

	Thing*			mThing = nullptr;
	Mine*			mMine = nullptr;

	NAS2D::Color	mColor = NAS2D::Color::Normal;

	bool			mExcavated = true;			/**< Used when a Digger uncovers underground tiles. */
	bool			mConnected = false;			/**< Flag indicating that this tile is connected to the Command Center. */
	bool			mThingIsStructure = false;	/**< Flag indicating that the Thing in the tile is a Structure. */
};
