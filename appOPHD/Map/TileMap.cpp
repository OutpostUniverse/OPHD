#include "TileMap.h"

#include "Tile.h"

#include "../MapObjects/Structure.h"

#include <libOPHD/EnumOreDepositYield.h>
#include <libOPHD/DirectionOffset.h>
#include <libOPHD/RandomNumberGenerator.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/Utility.h>
#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>
#include <NAS2D/Math/PointInRectangleRange.h>

#include <algorithm>
#include <functional>
#include <numeric>
#include <array>


using namespace NAS2D;


namespace {
	const std::string MapTerrainExtension = "_a.png";
	const auto MapSize = NAS2D::Vector{300, 150};


	constexpr std::size_t linearSize(NAS2D::Vector<int> size)
	{
		const auto converted = size.to<std::size_t>();
		return converted.x * converted.y;
	}


	constexpr std::size_t linearIndex(NAS2D::Point<int> point, int sizeX)
	{
		const auto converted = point.to<std::size_t>();
		return converted.x + static_cast<std::size_t>(sizeX) * converted.y;
	}


	std::vector<NAS2D::Point<int>> generateOreDeposits(NAS2D::Vector<int> mapSize, std::size_t oreDepositCount)
	{
		auto randPoint = [mapSize]() {
			return NAS2D::Point{
				randomNumber.generate<int>(5, mapSize.x - 5),
				randomNumber.generate<int>(5, mapSize.y - 5)
			};
		};

		std::vector<NAS2D::Point<int>> locations;
		locations.reserve(oreDepositCount);

		// Some locations might not be acceptable, so try up to twice as many locations
		// A high density of ore deposits could result in many rejected locations
		// Don't try indefinitely to avoid possibility of infinite loop
		std::vector<bool> usedLocations(linearSize(mapSize));
		for (std::size_t i = 0; (locations.size() < oreDepositCount) && (i < oreDepositCount * 2); ++i)
		{
			// Generate a location and check surroundings for minimum spacing
			const auto point = randPoint();
			if (!usedLocations[linearIndex(point, mapSize.x)])
			{
				locations.push_back(point);
				for (const auto& offset : DirectionScan3x3)
				{
					const auto usedPoint = point + offset;
					usedLocations[linearIndex(usedPoint, mapSize.x)] = true;
				}
			}
		}

		return locations;
	}


	void placeOreDeposits(TileMap& tileMap, const std::vector<NAS2D::Point<int>>& locations, const TileMap::OreDepositYields& oreDepositYields)
	{
		const auto total = std::accumulate(oreDepositYields.begin(), oreDepositYields.end(), 0);

		const auto randYield = [oreDepositYields, total]() {
			const auto randValue = randomNumber.generate<int>(1, total);
			return (randValue <= oreDepositYields[0]) ? OreDepositYield::Low :
				(randValue <= oreDepositYields[0] + oreDepositYields[1]) ? OreDepositYield::Medium :
				OreDepositYield::High;
		};

		for (const auto& location : locations)
		{
			auto& tile = tileMap.getTile({location, 0});
			tile.placeOreDeposit(new OreDeposit(randYield()));
			tile.bulldoze();
		}
	}
}


TileMap::TileMap(const std::string& mapPath, int maxDepth, std::size_t oreDepositCount, const OreDepositYields& oreDepositYields) :
	TileMap{mapPath, maxDepth}
{
	mOreDepositLocations = generateOreDeposits(mSizeInTiles, oreDepositCount);
	placeOreDeposits(*this, mOreDepositLocations, oreDepositYields);
}


TileMap::TileMap(const std::string& mapPath, int maxDepth) :
	mSizeInTiles{MapSize},
	mMaxDepth{maxDepth}
{
	buildTerrainMap(mapPath);
}


TileMap::~TileMap()
{
}


void TileMap::removeOreDepositLocation(const NAS2D::Point<int>& pt)
{
	auto& tile = getTile({pt, 0});
	if (!tile.hasOreDeposit())
	{
		throw std::runtime_error("No ore deposit found to remove");
	}

	mOreDepositLocations.erase(find(mOreDepositLocations.begin(), mOreDepositLocations.end(), pt));
	tile.placeOreDeposit(nullptr);
}


bool TileMap::isValidPosition(const MapCoordinate& position) const
{
	return NAS2D::Rectangle{{0, 0}, mSizeInTiles}.contains(position.xy) && position.z >= 0 && position.z <= mMaxDepth;
}


const Tile& TileMap::getTile(const MapCoordinate& position) const
{
	if (!isValidPosition(position))
	{
		throw std::runtime_error("Tile coordinates out of bounds: {" + std::to_string(position.xy.x) + ", " + std::to_string(position.xy.y) + ", " + std::to_string(position.z) + "}");
	}
	return mTileMap[linearIndex(position)];
}


Tile& TileMap::getTile(const MapCoordinate& position)
{
	const auto& constThis = *this;
	return const_cast<Tile&>(constThis.getTile(position));
}


void TileMap::buildTerrainMap(const std::string& path)
{
	const Image heightmap(path + MapTerrainExtension);

	mTileMap.resize(linearSize());

	/**
	 * Builds a terrain map based on the pixel color values in
	 * a maps height map.
	 *
	 * Height maps by default are in grey-scale. This method assumes
	 * that all channels are the same value so it only looks at the red.
	 * Color values are divided by 50 to get a height value from 1 - 4.
	 */
	for (int depth = 0; depth <= mMaxDepth; depth++)
	{
		for (const auto point : PointInRectangleRange{Rectangle{{0, 0}, mSizeInTiles}})
		{
			auto color = heightmap.pixelColor(point);
			auto& tile = getTile({point, depth});
			tile = {{point, depth}, static_cast<TerrainType>(color.red / 50)};
			if (depth > 0) { tile.excavated(false); }
		}
	}
}


void TileMap::serialize(NAS2D::Xml::XmlElement* element)
{
	// ==========================================
	// ORE DEPOSITS (MINES)
	// ==========================================
	auto* oreDeposits = new NAS2D::Xml::XmlElement("mines");
	element->linkEndChild(oreDeposits);

	for (const auto& location : mOreDepositLocations)
	{
		auto& oreDeposit = *getTile({location, 0}).oreDeposit();
		oreDeposits->linkEndChild(oreDeposit.serialize(location));
	}


	// ==========================================
	// TILES
	// ==========================================
	auto* tiles = new NAS2D::Xml::XmlElement("tiles");
	element->linkEndChild(tiles);

	// We're only writing out tiles that don't have structures or robots in them that are
	// underground and excavated or surface and bulldozed.
	for (int depth = 0; depth <= maxDepth(); ++depth)
	{
		for (const auto point : PointInRectangleRange{Rectangle{{0, 0}, mSizeInTiles}})
		{
			auto& tile = getTile({point, depth});
			if (
				((depth > 0 && tile.excavated()) || (tile.isBulldozed())) &&
				(tile.empty() && tile.oreDeposit() == nullptr)
			)
			{
				tiles->linkEndChild(
					NAS2D::dictionaryToAttributes(
						"tile",
						{{
							{"x", point.x},
							{"y", point.y},
							{"depth", depth},
							{"index", static_cast<int>(tile.index())},
						}}
					)
				);
			}
		}
	}
}


void TileMap::deserialize(NAS2D::Xml::XmlElement* element)
{
	// Ore deposits
	for (auto* oreDepositElement = element->firstChildElement("mines")->firstChildElement("mine"); oreDepositElement; oreDepositElement = oreDepositElement->nextSiblingElement())
	{
		const auto oreDepositDictionary = NAS2D::attributesToDictionary(*oreDepositElement);

		const auto x = oreDepositDictionary.get<int>("x");
		const auto y = oreDepositDictionary.get<int>("y");

		OreDeposit* oreDeposit = new OreDeposit();
		oreDeposit->deserialize(oreDepositElement);

		auto& tile = getTile({{x, y}, 0});
		tile.placeOreDeposit(oreDeposit);
		tile.bulldoze();

		mOreDepositLocations.push_back(Point{x, y});
	}

	// Tiles indexes
	for (auto* tileElement = element->firstChildElement("tiles")->firstChildElement("tile"); tileElement; tileElement = tileElement->nextSiblingElement())
	{
		const auto tileDictionary = NAS2D::attributesToDictionary(*tileElement);

		const auto x = tileDictionary.get<int>("x");
		const auto y = tileDictionary.get<int>("y");
		const auto depth = tileDictionary.get<int>("depth");
		const auto index = tileDictionary.get<int>("index");

		auto& tile = getTile({{x, y}, depth});
		tile.index(static_cast<TerrainType>(index));

		if (depth > 0) { tile.excavated(true); }
	}
}


/**
 * Implements MicroPather interface.
 *
 * \warning	Assumes stateStart and stateEnd are never nullptr.
 */
float TileMap::LeastCostEstimate(void* stateStart, void* stateEnd)
{
	return sqrtf(static_cast<float>((static_cast<Tile*>(stateEnd)->xy() - static_cast<Tile*>(stateStart)->xy()).lengthSquared()));
}


void TileMap::AdjacentCost(void* state, std::vector<micropather::StateCost>* adjacent)
{
	auto& tile = *static_cast<Tile*>(state);
	const auto tilePosition = tile.xy();

	for (const auto& offset : DirectionClockwise4)
	{
		const auto position = tilePosition + offset;
		if (!NAS2D::Rectangle{{0, 0}, mSizeInTiles}.contains(position))
		{
			continue;
		}

		auto& adjacentTile = getTile({position, 0});
		float cost = adjacentTile.movementCost();

		micropather::StateCost nodeCost = {&adjacentTile, cost};
		adjacent->push_back(nodeCost);
	}
}


bool TileMap::isTileBlockedByOreDeposit(const Tile& tile) const
{
	return getTile({tile.xy(), 0}).hasOreDeposit();
}


std::size_t TileMap::linearSize() const
{
	const auto convertedSize = mSizeInTiles.to<std::size_t>();
	const auto adjustedZ = mMaxDepth + 1;
	return convertedSize.x * convertedSize.y * static_cast<std::size_t>(adjustedZ);
}



std::size_t TileMap::linearIndex(const MapCoordinate& position) const
{
	const auto convertedSize = mSizeInTiles.to<std::size_t>();
	const auto convertedPosition = position.xy.to<std::size_t>();
	const auto convertedZ = static_cast<std::size_t>(position.z);
	return ((convertedZ * convertedSize.y) + convertedPosition.y) * convertedSize.x + convertedPosition.x;
}
