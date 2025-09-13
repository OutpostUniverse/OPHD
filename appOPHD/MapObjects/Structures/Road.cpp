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


	std::string roadAnimationName(ConnectorDir connectorDir, IntegrityLevel integrityLevel)
	{
		const std::string tag = (integrityLevel == IntegrityLevel::Destroyed) ? "-destroyed" :
			(integrityLevel < IntegrityLevel::Good) ? "-decayed" : "";
		return ConnectorDirStringTable.at(connectorDir) + tag;
	}
}


void Road::updateConnections(const TileMap& tileMap)
{
	const auto connectorDir = roadConnectorDir(tileMap, xyz());
	const auto integrityLevel = this->integrityLevel();
	mSprite.play(roadAnimationName(connectorDir, integrityLevel));
}
