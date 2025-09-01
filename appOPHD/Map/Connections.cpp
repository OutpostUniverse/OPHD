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


	constexpr std::array binaryEncodedIndexToConnectorDir = {
		ConnectorDir::Intersection, // None
		ConnectorDir::NorthSouth, // North
		ConnectorDir::EastWest, // East
		ConnectorDir::Intersection, // East + North
		ConnectorDir::NorthSouth, // South
		ConnectorDir::NorthSouth, // South + North
		ConnectorDir::Intersection, // South + East
		ConnectorDir::Intersection, // South + East + North
		ConnectorDir::EastWest, // West
		ConnectorDir::Intersection, // West + North
		ConnectorDir::EastWest, // West + East
		ConnectorDir::Intersection, // West + East + North
		ConnectorDir::Intersection, // West + South
		ConnectorDir::Intersection, // West + South + North
		ConnectorDir::Intersection, // West + South + East
		ConnectorDir::Intersection, // West + South + East + North
	};


	template <typename Predicate>
	std::size_t connectionBinaryEncodedIndex(const TileMap& tileMap, const MapCoordinate& mapCoordinate, Predicate predicate)
	{
		std::size_t index = 0;
		std::size_t directionValue = 1;
		for (const auto& offset : DirectionClockwise4)
		{
			const auto adjacentCoordinate = mapCoordinate.translate(offset);
			if (tileMap.isValidPosition(adjacentCoordinate) && predicate(tileMap.getTile(adjacentCoordinate)))
			{
				index += directionValue;
			}
			directionValue *= 2;
		}
		return index;
	}


	std::size_t roadConnectionBinaryEncodedIndex(const TileMap& tileMap, const MapCoordinate& mapCoordinate)
	{
		const auto isRoadAdjacent = [](const Tile& tile) { return tile.hasStructure() && tile.structure()->isRoad(); };
		return connectionBinaryEncodedIndex(tileMap, mapCoordinate, isRoadAdjacent);
	}


	ConnectorDir roadConnectorDir(const TileMap& tileMap, const MapCoordinate& mapCoordinate)
	{
		return binaryEncodedIndexToConnectorDir.at(roadConnectionBinaryEncodedIndex(tileMap, mapCoordinate));
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
