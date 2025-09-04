#include "GraphWalker.h"

#include "Map/Tile.h"
#include "Map/TileMap.h"
#include "MapObjects/Structure.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumDirection.h>
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
	thisTile.structure()->connected(true);

	const auto directions = std::array{
		Direction::Up,
		Direction::Down,
		Direction::North,
		Direction::East,
		Direction::South,
		Direction::West,
	};

	for (const auto direction : directions)
	{
		const auto nextPosition = position.translate(direction);
		if (!tileMap.isValidPosition(nextPosition)) { continue; }

		auto& nextTile = tileMap.getTile(nextPosition);
		if (!nextTile.hasStructure() || nextTile.structure()->connected()) { continue; }

		// At least one end must be a Tube or AirShaft as other Structures don't connect to each other
		if (thisTile.structure()->isConnector() || nextTile.structure()->isConnector())
		{
			walkGraph(nextPosition, tileMap);
		}
	}
}
