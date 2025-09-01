#include "Road.h"

#include "../../Constants/Numbers.h"
#include "../../Map/Connections.h"

#include <libOPHD/EnumConnectorDir.h>

#include <string>


namespace
{
	const std::map<ConnectorDir, std::string> ConnectorDirStringTable =
	{
		{ConnectorDir::Intersection, "intersection"},
		{ConnectorDir::NorthSouth, "left"},
		{ConnectorDir::EastWest, "right"},
	};


	std::string roadAnimationName(const Road& road, const TileMap& tileMap)
	{
		const auto connectorDir = roadConnectorDir(tileMap, road.xyz());
		const auto integrity = road.integrity();
		const std::string tag = (integrity == 0) ? "-destroyed" :
			(integrity < constants::RoadIntegrityChange) ? "-decayed" : "";
		return ConnectorDirStringTable.at(connectorDir) + tag;
	}
}


void Road::updateConnections(const TileMap& tileMap)
{
	if (operational())
	{
		mSprite.play(roadAnimationName(*this, tileMap));
	}
}
