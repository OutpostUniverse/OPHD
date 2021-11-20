#pragma once

#include "Tile.h"

#include "../MicroPather/micropather.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Resource/Image.h>

#include <string>
#include <vector>
#include <array>
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
	using MineYields = std::array<int, 3>; // {low, med, high}

	TileMap(const std::string& mapPath, int maxDepth, int mineCount, const MineYields& mineYields);
	TileMap(const std::string& mapPath, int maxDepth);
	TileMap(const TileMap&) = delete;
	TileMap& operator=(const TileMap&) = delete;

	NAS2D::Vector<int> size() const { return mSizeInTiles; }
	int maxDepth() const { return mMaxDepth; }

	bool isValidPosition(const MapCoordinate& position) const;

	const Tile& getTile(const MapCoordinate& position) const;
	Tile& getTile(const MapCoordinate& position);

	const std::vector<NAS2D::Point<int>>& mineLocations() const { return mMineLocations; }
	void removeMineLocation(const NAS2D::Point<int>& pt);

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

	std::pair<void*, void*> mPathStartEndPair = {nullptr, nullptr};
};
