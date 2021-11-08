#pragma once

#include "Tile.h"

#include "../States/Planet.h"
#include "../MicroPather/micropather.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Resource/Image.h>

#include <string>
#include <vector>
#include <utility>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}
}

enum class Direction;


class TileMap : public micropather::Graph
{
public:
	TileMap(const std::string& mapPath, const std::string& tilesetPath, int maxDepth, int mineCount, Planet::Hostility hostility, bool setupMines = true);
	TileMap(const TileMap&) = delete;
	TileMap& operator=(const TileMap&) = delete;

	NAS2D::Vector<int> size() const { return mSizeInTiles; }
	int maxDepth() const { return mMaxDepth; }

	bool isValidPosition(const MapCoordinate& position) const;

	const Tile& getTile(const MapCoordinate& position) const;
	Tile& getTile(const MapCoordinate& position);
	Tile& getTile(NAS2D::Point<int> position) { return getTile({position, mOriginTilePosition.z}); }

	const std::vector<NAS2D::Point<int>>& mineLocations() const { return mMineLocations; }
	void removeMineLocation(const NAS2D::Point<int>& pt);

	NAS2D::Rectangle<int> viewArea() const;
	void mapViewLocation(NAS2D::Point<int> point);
	void mapViewLocation(const MapCoordinate& position);
	void centerOn(NAS2D::Point<int> point);
	void centerOn(const MapCoordinate& position);
	void moveView(Direction direction);

	int currentDepth() const { return mOriginTilePosition.z; }
	void currentDepth(int i);

	bool isVisibleTile(const MapCoordinate& position) const;
	bool isMouseOverTile() const;

	MapCoordinate mouseTilePosition() const { return {mMouseTilePosition, mOriginTilePosition.z}; }
	Tile& mouseTile();

	int edgeLength() const { return mEdgeLength; }

	void onMouseMove(NAS2D::Point<int> position);
	void onResize(NAS2D::Vector<int>);

	void update();
	void draw() const;

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);


	/** MicroPather public interface implementation. */
	float LeastCostEstimate(void* stateStart, void* stateEnd) override;
	void AdjacentCost(void* state, std::vector<micropather::StateCost>* adjacent) override;
	void PrintStateInfo(void* /*state*/) override {}

	void pathStartAndEnd(void* start, void* end);

private:
	void buildTerrainMap(const std::string& path);


	const NAS2D::Vector<int> mSizeInTiles;
	const int mMaxDepth = 0;
	std::vector<Tile> mTileMap;
	std::vector<NAS2D::Point<int>> mMineLocations;

	std::string mMapPath;
	std::string mTsetPath;

	const NAS2D::Image mTileset;
	const NAS2D::Image mMineBeacon;

	int mEdgeLength = 0;

	MapCoordinate mOriginTilePosition{{0, 0}, 0}; // Top tile of detail view diamond, or top left corner of minimap view box
	NAS2D::Point<int> mOriginPixelPosition; // Top pixel at top of diamond

	NAS2D::Point<int> mMouseTilePosition;

	std::pair<void*, void*> mPathStartEndPair = {nullptr, nullptr};
};
