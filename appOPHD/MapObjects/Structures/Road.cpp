#include "Road.h"

#include "../../Map/Connections.h"

#include <libOPHD/EnumConnectorDir.h>
#include <libOPHD/EnumIntegrityLevel.h>

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
		const auto integrityLevel = road.integrityLevel();
		const std::string tag = (integrityLevel == IntegrityLevel::Destroyed) ? "-destroyed" :
			(integrityLevel < IntegrityLevel::Good) ? "-decayed" : "";
		return ConnectorDirStringTable.at(connectorDir) + tag;
	}
}


void Road::updateConnections(const TileMap& tileMap)
{
	mSprite.play(roadAnimationName(*this, tileMap));
}
