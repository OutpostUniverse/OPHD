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


	auto getSurroundingRoads(const TileMap& tileMap, const NAS2D::Point<int>& tileLocation)
	{
		std::array<bool, 4> surroundingTiles{false, false, false, false};
		for (size_t i = 0; i < 4; ++i)
		{
			const auto tileToInspect = tileLocation + DirectionClockwise4[i];
			const auto surfacePosition = MapCoordinate{tileToInspect, 0};
			if (!tileMap.isValidPosition(surfacePosition)) { continue; }
			const auto& tile = tileMap.getTile(surfacePosition);
			if (!tile.hasStructure()) { continue; }

			surroundingTiles[i] = tile.structure()->isRoad();
		}
		return surroundingTiles;
	}


	std::string roadAnimationName(int integrity, const std::array<bool, 4>& surroundingTiles)
	{
		const std::string tag = (integrity == 0) ? "-destroyed" :
			(integrity < constants::RoadIntegrityChange) ? "-decayed" : "";
		return ConnectorDirStringTable.at(IntersectionPatternTable.at(surroundingTiles)) + tag;
	}
}


std::string roadAnimationName(const Road& road, const TileMap& tileMap)
{
	const auto tileLocation = road.xyz().xy;
	const auto surroundingTiles = getSurroundingRoads(tileMap, tileLocation);
	return roadAnimationName(road.integrity(), surroundingTiles);
}
