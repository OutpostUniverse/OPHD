#include "Tube.h"

#include "../../Map/Connections.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumConnectorDir.h>

#include <string>
#include <stdexcept>


namespace
{
	const std::string TubeIntersection = "intersection";
	const std::string TubeRight = "right";
	const std::string TubeLeft = "left";


	const std::string& getAnimationName(ConnectorDir dir)
	{
		return
			(dir == ConnectorDir::Intersection) ? TubeIntersection :
			(dir == ConnectorDir::EastWest) ? TubeRight :
			(dir == ConnectorDir::NorthSouth) ? TubeLeft :
			throw std::runtime_error("Tried to create a Tube structure with invalid connector direction parameter.");
	}
}


Tube::Tube(Tile& tile) :
	Structure{StructureID::Tube, tile}
{
}


void Tube::updateConnections(const TileMap& tileMap)
{
	const auto connectorDir = tubeConnectorDir(tileMap, xyz());
	mSprite.play(getAnimationName(connectorDir));
}
