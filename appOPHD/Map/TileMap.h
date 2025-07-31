#pragma once

#include <NAS2D/Math/Vector.h>

#include <string>
#include <vector>
#include <array>


namespace NAS2D
{
	namespace Xml
	{
		class XmlElement;
	}

	template <typename BaseType> struct Point;
}

enum class Direction;
struct MapCoordinate;
class Tile;


class TileMap
{
public:
	using OreDepositYields = std::array<int, 3>; // {low, med, high}

	TileMap(const std::string& mapPath, int maxDepth, std::size_t oreDepositCount, const OreDepositYields& oreDepositYields);
	TileMap(const std::string& mapPath, int maxDepth);
	TileMap(const TileMap&) = delete;
	TileMap& operator=(const TileMap&) = delete;
	~TileMap();

	NAS2D::Vector<int> size() const { return mSizeInTiles; }
	int maxDepth() const { return mMaxDepth; }

	bool isValidPosition(const MapCoordinate& position) const;

	const Tile& getTile(const MapCoordinate& position) const;
	Tile& getTile(const MapCoordinate& position);

	const std::vector<NAS2D::Point<int>>& oreDepositLocations() const;
	void removeOreDepositLocation(const NAS2D::Point<int>& pt);

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);

	bool hasOreDeposit(const MapCoordinate& mapCoordinate) const;

private:
	std::size_t linearSize() const;
	std::size_t linearIndex(const MapCoordinate& position) const;

	void buildTerrainMap(const std::string& path);


	const NAS2D::Vector<int> mSizeInTiles;
	const int mMaxDepth = 0;
	std::vector<Tile> mTileMap;
	std::vector<NAS2D::Point<int>> mOreDepositLocations;
};
