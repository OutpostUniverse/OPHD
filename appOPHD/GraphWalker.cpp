#include "GraphWalker.h"

#include "Map/Tile.h"
#include "Map/TileMap.h"
#include "MapObjects/Structure.h"

#include <libOPHD/DirectionOffset.h>
#include <libOPHD/Map/MapCoordinate.h>


void walkGraph(const std::vector<MapCoordinate>& positions, TileMap& tileMap)
{
	for (const auto& position : positions)
	{
		walkGraph(position, tileMap);
	}
}


void walkGraph(const MapCoordinate& position, TileMap& tileMap)
{
	Tile& thisTile = tileMap.getTile(position);
	auto& thisStructure = *thisTile.structure();
	thisStructure.connected(true);

	for (const auto& direction : MapOffsetClockwise6)
	{
		const auto nextPosition = position.translate(direction);
		if (!tileMap.isValidPosition(nextPosition)) { continue; }

		auto& nextTile = tileMap.getTile(nextPosition);
		if (!nextTile.hasStructure()) { continue; }

		const auto& nextStructure = *nextTile.structure();
		if (nextStructure.connected()) { continue; }

		// At least one end must be a Tube or AirShaft as other Structures don't connect to each other
		if (thisStructure.isConnector() || nextStructure.isConnector())
		{
			walkGraph(nextPosition, tileMap);
		}
	}
}
