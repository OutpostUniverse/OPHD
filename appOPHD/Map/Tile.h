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
	void excavate() { mExcavated = true; }

	bool isImpassable() const;

	MapObject* mapObject() const { return mMapObject; }
	void mapObject(MapObject*);
	void removeMapObject();

	bool hasOreDeposit() const { return mOreDeposit != nullptr; }
	bool hasMapObject() const { return mMapObject != nullptr; }
	bool hasStructure() const { return structure() != nullptr; }
	bool hasRobot() const { return robot() != nullptr; }

	Structure* structure() const;
	Robot* robot() const;

	const OreDeposit* oreDeposit() const { return mOreDeposit; }
	OreDeposit* oreDeposit() { return mOreDeposit; }
	void placeOreDeposit(OreDeposit*);
	void removeOreDeposit();

	void overlay(Overlay overlay) { mOverlay = overlay; }
	Overlay overlay() const { return mOverlay; }

private:
	TerrainType mIndex;
	bool mExcavated = false;
	MapObject* mMapObject = nullptr;
	OreDeposit* mOreDeposit = nullptr;
	MapCoordinate mPosition;
	Overlay mOverlay{Overlay::None};
};
