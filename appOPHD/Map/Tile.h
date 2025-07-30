#pragma once

#include <libOPHD/Map/MapCoordinate.h>

#include <NAS2D/Math/Point.h>


enum class TerrainType;
class OreDeposit;
class MapObject;
class Robot;
class Structure;


class Tile
{
public:
	enum class Overlay
	{
		Communications,
		Connectedness,
		TruckingRoutes,
		Police,

		None
	};

public:
	Tile();
	Tile(const MapCoordinate& position, TerrainType);
	Tile(const Tile&) = delete;
	Tile& operator=(const Tile&) = delete;
	Tile(Tile&&) noexcept;
	Tile& operator=(Tile&&) noexcept;
	~Tile();

	TerrainType index() const { return mIndex; }
	void index(TerrainType index) { mIndex = index; }

	const MapCoordinate& xyz() const { return mPosition; }
	NAS2D::Point<int> xy() const { return mPosition.xy; }
	int depth() const { return mPosition.z; }
	void depth(int i) { mPosition.z = i; }

	bool isSurface() const;

	bool isBulldozed() const;
	void bulldoze();

	bool excavated() const { return mExcavated; }
	void excavated(bool value) { mExcavated = value; }

	bool isImpassable() const;

	void mapObject(MapObject*);
	MapObject* mapObject() const { return mMapObject; }

	void removeMapObject();

	bool empty() const { return mMapObject == nullptr; }

	bool hasOreDeposit() const { return mOreDeposit != nullptr; }
	bool hasStructure() const { return structure() != nullptr; }
	bool hasRobot() const { return robot() != nullptr; }

	Structure* structure() const;
	Robot* robot() const;

	const OreDeposit* oreDeposit() const { return mOreDeposit; }
	OreDeposit* oreDeposit() { return mOreDeposit; }
	void placeOreDeposit(OreDeposit*);

	void overlay(Overlay overlay) { mOverlay = overlay; }
	Overlay overlay() const { return mOverlay; }

private:
	TerrainType mIndex;

	MapCoordinate mPosition;

	MapObject* mMapObject = nullptr;
	OreDeposit* mOreDeposit = nullptr;

	Overlay mOverlay{Overlay::None};

	bool mExcavated = true; /**< Used when a Digger uncovers underground tiles. */
};
