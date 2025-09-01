#include "Connections.h"

#include "Tile.h"
#include "TileMap.h"
#include "../MapObjects/Structures/Road.h"
#include "../Constants/Numbers.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/DirectionOffset.h>
#include <libOPHD/Map/MapCoordinate.h>

#include <NAS2D/Math/Point.h>


namespace
{
	const std::map<ConnectorDir, std::string> ConnectorDirStringTable =
	{
		{ConnectorDir::Intersection, "intersection"},
		{ConnectorDir::NorthSouth, "left"},
		{ConnectorDir::EastWest, "right"},
	};


	const std::map<std::array<bool, 4>, ConnectorDir> IntersectionPatternTable =
	{
		{{true, false, true, false}, ConnectorDir::NorthSouth},
		{{true, false, false, false}, ConnectorDir::NorthSouth},
		{{false, false, true, false}, ConnectorDir::NorthSouth},

		{{false, true, false, true}, ConnectorDir::EastWest},
		{{false, true, false, false}, ConnectorDir::EastWest},
		{{false, false, false, true}, ConnectorDir::EastWest},

		{{false, false, false, false}, ConnectorDir::Intersection},
		{{true, true, false, false}, ConnectorDir::Intersection},
		{{false, false, true, true}, ConnectorDir::Intersection},
		{{false, true, true, true}, ConnectorDir::Intersection},
		{{true, true, true, false}, ConnectorDir::Intersection},
		{{true, true, true, true}, ConnectorDir::Intersection},
		{{true, false, false, true}, ConnectorDir::Intersection},
		{{false, true, true, false}, ConnectorDir::Intersection},

		{{false, true, true, true}, ConnectorDir::Intersection},
		{{true, false, true, true}, ConnectorDir::Intersection},
		{{true, true, false, true}, ConnectorDir::Intersection},
		{{true, true, true, false}, ConnectorDir::Intersection}
	};


	template <typename Predicate>
	auto getSurroundingConnections(const TileMap& tileMap, const MapCoordinate& mapCoordinate, Predicate predicate)
	{
		std::array<bool, 4> surroundingTiles{false, false, false, false};
		for (size_t i = 0; i < 4; ++i)
		{
			const auto adjacentCoordinate = mapCoordinate.translate(DirectionClockwise4[i]);
			surroundingTiles[i] = tileMap.isValidPosition(adjacentCoordinate) && predicate(tileMap.getTile(adjacentCoordinate));
		}
		return surroundingTiles;
	}


	auto getSurroundingRoads(const TileMap& tileMap, const MapCoordinate& mapCoordinate)
	{
		const auto isRoadAdjacent = [](const Tile& tile) { return tile.hasStructure() && tile.structure()->isRoad(); };
		return getSurroundingConnections(tileMap, mapCoordinate, isRoadAdjacent);
	}


	ConnectorDir roadConnectorDir(const TileMap& tileMap, const MapCoordinate& mapCoordinate)
	{
		return IntersectionPatternTable.at(getSurroundingRoads(tileMap, mapCoordinate));
	}


	std::string roadAnimationName(int integrity, ConnectorDir connectorDir)
	{
		const std::string tag = (integrity == 0) ? "-destroyed" :
			(integrity < constants::RoadIntegrityChange) ? "-decayed" : "";
		return ConnectorDirStringTable.at(connectorDir) + tag;
	}
}


std::string roadAnimationName(const Road& road, const TileMap& tileMap)
{
	const auto connectorDir = roadConnectorDir(tileMap, road.xyz());
	return roadAnimationName(road.integrity(), connectorDir);
}
