#pragma once

#include "Tile.h"

#include "../States/Planet.h"
#include "../MicroPather/micropather.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

enum class Direction;

using Point2dList = std::vector<NAS2D::Point<int>>;


class TileMap : public micropather::Graph
{
public:
	enum TileMapLevel
	{
		LEVEL_SURFACE = 0,
		LEVEL_UG_1,
		LEVEL_UG_2,
		LEVEL_UG_3,
		LEVEL_UG_4
	};


	TileMap(const std::string& mapPath, const std::string& tilesetPath, int maxDepth, int mineCount, Planet::Hostility hostility /*= constants::Hostility::None*/, bool setupMines = true);
	TileMap(const TileMap&) = delete;
	TileMap& operator=(const TileMap&) = delete;

	NAS2D::Vector<int> size() const { return mSizeInTiles; }
	int maxDepth() const { return mMaxDepth; }

	bool isValidPosition(const MapCoordinate& position) const;

	Tile& getTile(const MapCoordinate& position);
	Tile& getTile(NAS2D::Point<int> position) { return getTile({position, mMouseTilePosition.z}); }

	const Point2dList& mineLocations() const { return mMineLocations; }
	void removeMineLocation(const NAS2D::Point<int>& pt);

	const NAS2D::Point<int>& mapViewLocation() const { return mOriginTilePosition; }
	void mapViewLocation(NAS2D::Point<int> point);
	void mapViewLocation(const MapCoordinate& position);
	void centerOn(NAS2D::Point<int> point);
	void centerOn(const MapCoordinate& position);
	void moveView(Direction direction);

	int currentDepth() const { return mMouseTilePosition.z; }
	void currentDepth(int i);

	bool isVisibleTile(const MapCoordinate& position) const;
	bool tileHighlightVisible() const;

	const MapCoordinate& mouseTilePosition() const { return mMouseTilePosition; }
	Tile* getVisibleTile(const MapCoordinate& position);
	Tile* getVisibleTile() { return getVisibleTile(mouseTilePosition()); }

	int edgeLength() const { return mEdgeLength; }
	const NAS2D::Rectangle<int>& boundingBox() const { return mMapBoundingBox; }

	void injectMouse(NAS2D::Point<int> position) { mMousePixelPosition = position; }

	void initMapDrawParams(NAS2D::Vector<int>);

	void update();
	void draw();

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);


	/** MicroPather public interface implementation. */
	float LeastCostEstimate(void* stateStart, void* stateEnd) override;
	void AdjacentCost(void* state, std::vector<micropather::StateCost>* adjacent) override;
	void PrintStateInfo(void* /*state*/) override {}

	void pathStartAndEnd(void* start, void* end);

protected:
	enum MouseMapRegion
	{
		MMR_MIDDLE,
		MMR_TOP_RIGHT,
		MMR_TOP_LEFT,
		MMR_BOTTOM_RIGHT,
		MMR_BOTTOM_LEFT
	};

	std::vector<std::vector<MouseMapRegion>> mMouseMap;

private:
	using TileGrid = std::vector<std::vector<Tile>>;
	using TileArray = std::vector<TileGrid>;

	void buildMouseMap();
	void buildTerrainMap(const std::string& path);
	void setupMines(int, Planet::Hostility);
	void addMineSet(NAS2D::Point<int> suggestedMineLocation, Point2dList& plist, MineProductionRate rate);
	NAS2D::Point<int> findSurroundingMineLocation(NAS2D::Point<int> centerPoint);

	void updateTileHighlight();

	MouseMapRegion getMouseMapRegion(int x, int y);


	const NAS2D::Vector<int> mSizeInTiles;
	const int mMaxDepth = 0;
	TileArray mTileMap;
	Point2dList mMineLocations;

	std::string mMapPath;
	std::string mTsetPath;

	const NAS2D::Image mTileset;
	const NAS2D::Image mMineBeacon;

	NAS2D::Timer mTimer;

	int mEdgeLength = 0;

	MapCoordinate mMouseTilePosition{};
	NAS2D::Point<int> mMousePixelPosition;

	NAS2D::Point<int> mOriginTilePosition; // Top tile of detail view diamond, or top left corner of minimap view box
	NAS2D::Point<int> mOriginPixelPosition; // Top left pixel of tile at top of diamond

	NAS2D::Rectangle<int> mMapBoundingBox; // Tightest pixel area containing all drawn tiles

	std::pair<void*, void*> mPathStartEndPair = {nullptr, nullptr};
};
