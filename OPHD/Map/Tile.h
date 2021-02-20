#pragma once

#include "../Common.h"

#include <NAS2D/Renderer/Point.h>
#include <NAS2D/Renderer/Vector.h>


class Mine;
class Thing;
class Robot;
class Structure;


class Tile
{
public:
	Tile() = default;
	Tile(NAS2D::Point<int> position, int depth, TerrainType index);
	Tile(const Tile& other) = delete;
	Tile& operator=(const Tile& other) = delete;
	Tile(Tile&& other) noexcept;
	Tile& operator=(Tile&& other) noexcept;
	~Tile();

	TerrainType index() const { return mIndex; }
	void index(TerrainType index) { mIndex = index; }

	NAS2D::Point<int> position() const { return mPosition; }

	int depth() const { return mDepth; }
	void depth(int i) { mDepth = i; }

	bool bulldozed() const { return index() == TerrainType::Dozed; }

	bool excavated() const { return mExcavated; }
	void excavated(bool value) { mExcavated = value; }

	bool connected() const { return mConnected; }
	void connected(bool value) { mConnected = value; }

	Thing* thing() { return mThing; }

	bool empty() const { return mThing == nullptr; }

	bool hasMine() const { return mMine != nullptr; }

	Structure* structure();
	Robot* robot();

	bool thingIsStructure() const { return mThingIsStructure; }
	bool thingIsRobot() { return robot() != nullptr; }

	void pushThing(Thing* thing);
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
	TerrainType mIndex = TerrainType::Dozed;

	NAS2D::Point<int> mPosition; /**< Tile Position Information */
	int mDepth = 0; /**< Tile Position Information */

	Thing* mThing = nullptr;
	Mine* mMine = nullptr;

	NAS2D::Color mColor = NAS2D::Color::Normal;

	bool mExcavated = true; /**< Used when a Digger uncovers underground tiles. */
	bool mConnected = false; /**< Flag indicating that this tile is connected to the Command Center. */
	bool mThingIsStructure = false; /**< Flag indicating that the Thing in the tile is a Structure. */
};
