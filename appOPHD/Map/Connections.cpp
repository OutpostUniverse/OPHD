#include "Connections.h"

#include "Tile.h"
#include "TileMap.h"
#include "../StructureManager.h"
#include "../MapObjects/Structures/Road.h"
#include "../Constants/Numbers.h"

#include <libOPHD/DirectionOffset.h>
#include <libOPHD/Map/MapCoordinate.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Math/Point.h>


namespace
{
	const std::map<std::array<bool, 4>, std::string> IntersectionPatternTable =
	{
		{{true, false, true, false}, "left"},
		{{true, false, false, false}, "left"},
		{{false, false, true, false}, "left"},

		{{false, true, false, true}, "right"},
		{{false, true, false, false}, "right"},
		{{false, false, false, true}, "right"},

		{{false, false, false, false}, "intersection"},
		{{true, true, false, false}, "intersection"},
		{{false, false, true, true}, "intersection"},
		{{false, true, true, true}, "intersection"},
		{{true, true, true, false}, "intersection"},
		{{true, true, true, true}, "intersection"},
		{{true, false, false, true}, "intersection"},
		{{false, true, true, false}, "intersection"},

		{{false, true, true, true}, "intersection"},
		{{true, false, true, true}, "intersection"},
		{{true, true, false, true}, "intersection"},
		{{true, true, true, false}, "intersection"}
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

			surroundingTiles[i] = tile.structure()->structureId() == StructureID::SID_ROAD;
		}
		return surroundingTiles;
	}


	std::string roadAnimationName(int integrity, const std::array<bool, 4>& surroundingTiles)
	{
		const std::string tag = (integrity == 0) ? "-destroyed" :
			(integrity < constants::RoadIntegrityChange) ? "-decayed" : "";
		return IntersectionPatternTable.at(surroundingTiles) + tag;
	}
}


std::string roadAnimationName(const Road& road, const TileMap& tileMap)
{
	const auto tileLocation = NAS2D::Utility<StructureManager>::get().tileFromStructure(&road).xy();
	const auto surroundingTiles = getSurroundingRoads(tileMap, tileLocation);
	return roadAnimationName(road.integrity(), surroundingTiles);
}
