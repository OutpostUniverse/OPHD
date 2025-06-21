#pragma once

#include <libOPHD/EnumTerrainType.h>
#include <libOPHD/Map/MapCoordinate.h>

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>


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
	Tile() = default;
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

	bool isSurface();

	bool bulldozed() const { return index() == TerrainType::Dozed; }

	bool excavated() const { return mExcavated; }
	void excavated(bool value) { mExcavated = value; }

	MapObject* mapObject() const { return mMapObject; }

	bool empty() const { return mMapObject == nullptr; }

	bool hasOreDeposit() const { return mOreDeposit != nullptr; }

	Structure* structure() const;
	Robot* robot() const;

	bool hasStructure() const { return structure() != nullptr; }
	bool thingIsRobot() const { return robot() != nullptr; }

	void pushMapObject(MapObject*);
	void deleteMapObject();

	void removeMapObject();

	const OreDeposit* oreDeposit() const { return mOreDeposit; }
	OreDeposit* oreDeposit() { return mOreDeposit; }
	void placeOreDeposit(OreDeposit*);

	void overlay(Overlay overlay) { mOverlay = overlay; }
	Overlay overlay() const { return mOverlay; }

	float movementCost() const;

private:
	TerrainType mIndex = TerrainType::Dozed;

	MapCoordinate mPosition;

	MapObject* mMapObject = nullptr;
	OreDeposit* mOreDeposit = nullptr;

	Overlay mOverlay{Overlay::None};

	bool mExcavated = true; /**< Used when a Digger uncovers underground tiles. */
};
