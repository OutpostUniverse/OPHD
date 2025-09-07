#include "TileMap.h"

#include "Tile.h"

#include <libOPHD/EnumOreDepositYield.h>
#include <libOPHD/DirectionOffset.h>
#include <libOPHD/RandomNumberGenerator.h>
#include <libOPHD/MapObjects/OreDeposit.h>

#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/PointInRectangleRange.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Resource/Image.h>

#include <algorithm>
#include <functional>
#include <numeric>
#include <array>


namespace {
	const std::string MapTerrainExtension = "_a.png";
	const auto MapSize = NAS2D::Vector{300, 150};


	const std::array<std::string, 4> ResourceFieldNames =
	{
		"common_metals", "common_minerals", "rare_metals", "rare_minerals"
	};


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


	std::vector<OreDeposit*> placeOreDeposits(TileMap& tileMap, const std::vector<NAS2D::Point<int>>& locations, const TileMap::OreDepositYields& oreDepositYields)
	{
		std::vector<OreDeposit*> oreDeposits;
		oreDeposits.reserve(locations.size());

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
			auto* oreDeposit = oreDeposits.emplace_back(new OreDeposit(randYield(), location));
			tile.placeOreDeposit(oreDeposit);
			tile.bulldoze();
		}
		return oreDeposits;
	}


	NAS2D::Xml::XmlElement* serializeOreDeposit(const OreDeposit& oreDeposit)
	{
		const auto location = oreDeposit.location();
		auto* element = NAS2D::dictionaryToAttributes(
			"mine",
			{{
				{"x", location.x},
				{"y", location.y},
				{"depth", oreDeposit.digDepth()},
				{"yield", static_cast<int>(oreDeposit.yield())},
				// Unused fields, retained for backwards compatibility
				{"active", true},
				{"flags", "011111"},
			}}
		);

		const auto& availableResources = oreDeposit.availableResources();
		if (!availableResources.isEmpty())
		{
			element->linkEndChild(NAS2D::dictionaryToAttributes(
				"vein",
				{{
					{ResourceFieldNames[0], availableResources.resources[0]},
					{ResourceFieldNames[1], availableResources.resources[1]},
					{ResourceFieldNames[2], availableResources.resources[2]},
					{ResourceFieldNames[3], availableResources.resources[3]},
				}}
			));
		}

		return element;
	}


	OreDeposit deserializeOreDeposit(NAS2D::Xml::XmlElement* element)
	{
		const auto dictionary = NAS2D::attributesToDictionary(*element);

		const auto x = dictionary.get<int>("x");
		const auto y = dictionary.get<int>("y");
		const auto digDepth = dictionary.get<int>("depth");
		const auto yield = static_cast<OreDepositYield>(dictionary.get<int>("yield"));

		StorableResources availableResources = {};
		// Keep the vein iteration so we can still load old saved games
		for (auto* vein = element->firstChildElement(); vein != nullptr; vein = vein->nextSiblingElement())
		{
			const auto veinDictionary = NAS2D::attributesToDictionary(*vein);
			const auto veinReserves = StorableResources{
				veinDictionary.get<int>(ResourceFieldNames[0], 0),
				veinDictionary.get<int>(ResourceFieldNames[1], 0),
				veinDictionary.get<int>(ResourceFieldNames[2], 0),
				veinDictionary.get<int>(ResourceFieldNames[3], 0),
			};
			availableResources += veinReserves;
		}

		return {availableResources, yield, {x, y}, digDepth};
	}
}


TileMap::TileMap(const std::string& mapPath, int maxDepth, std::size_t oreDepositCount, const OreDepositYields& oreDepositYields) :
	TileMap{mapPath, maxDepth}
{
	mOreDeposits = placeOreDeposits(*this, generateOreDeposits(mSizeInTiles, oreDepositCount), oreDepositYields);
}


TileMap::TileMap(const std::string& mapPath, int maxDepth) :
	mSizeInTiles{MapSize},
	mMaxDepth{maxDepth}
{
	buildTerrainMap(mapPath);
}


TileMap::~TileMap()
{
	for (const auto* oreDeposit : mOreDeposits)
	{
		auto& tile = getTile({oreDeposit->location(), 0});
		tile.removeOreDeposit();
		delete oreDeposit;
	}
}


NAS2D::Rectangle<int> TileMap::area() const
{
	return {{0, 0}, mSizeInTiles};
}


bool TileMap::isValidPosition(const MapCoordinate& position) const
{
	return area().contains(position.xy) && position.z >= 0 && position.z <= mMaxDepth;
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


bool TileMap::hasOreDeposit(const MapCoordinate& mapCoordinate) const
{
	return getTile({mapCoordinate.xy, 0}).hasOreDeposit();
}


const std::vector<OreDeposit*>& TileMap::oreDeposits() const
{
	return mOreDeposits;
}


void TileMap::removeOreDepositLocation(const NAS2D::Point<int>& location)
{
	auto& tile = getTile({location, 0});
	if (!tile.hasOreDeposit())
	{
		throw std::runtime_error("No ore deposit found to remove");
	}

	mOreDeposits.erase(find_if(mOreDeposits.begin(), mOreDeposits.end(), [location](OreDeposit* oreDeposit){ return oreDeposit->location() == location; }));
	auto* oreDeposit = tile.oreDeposit();
	tile.removeOreDeposit();
	delete oreDeposit;
}


void TileMap::buildTerrainMap(const std::string& path)
{
	const NAS2D::Image heightmap(path + MapTerrainExtension);

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
		for (const auto point : NAS2D::PointInRectangleRange{area()})
		{
			auto color = heightmap.pixelColor(point);
			auto& tile = getTile({point, depth});
			tile = {{point, depth}, static_cast<TerrainType>(color.red / 50)};
			if (depth == 0) { tile.excavate(); }
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

	for (const auto* oreDeposit : mOreDeposits)
	{
		oreDeposits->linkEndChild(serializeOreDeposit(*oreDeposit));
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
		for (const auto point : NAS2D::PointInRectangleRange{area()})
		{
			auto& tile = getTile({point, depth});
			if (
				((depth > 0 && tile.excavated()) || tile.isBulldozed()) &&
				(!tile.hasMapObject() && !tile.hasOreDeposit())
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
		OreDeposit* oreDeposit = new OreDeposit(deserializeOreDeposit(oreDepositElement));

		auto& tile = getTile({oreDeposit->location(), 0});
		tile.placeOreDeposit(oreDeposit);
		tile.bulldoze();

		mOreDeposits.push_back(oreDeposit);
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

		if (depth > 0) { tile.excavate(); }
	}
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
