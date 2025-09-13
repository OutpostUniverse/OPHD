#include "Tube.h"

#include "../StructureState.h"
#include "../../Constants/Strings.h"
#include "../../Map/Connections.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumConnectorDir.h>

#include <stdexcept>


namespace
{
	const std::string& getAnimationName(ConnectorDir dir)
	{
		return
			(dir == ConnectorDir::Intersection) ? constants::TubeIntersection :
			(dir == ConnectorDir::EastWest) ? constants::TubeRight :
			(dir == ConnectorDir::NorthSouth) ? constants::TubeLeft :
			throw std::runtime_error("Tried to create a Tube structure with invalid connector direction parameter.");
	}
}


Tube::Tube(Tile& tile) :
	Structure{StructureID::Tube, tile}
{
	mStructureState = StructureState::Operational;
}


void Tube::updateConnections(const TileMap& tileMap)
{
	const auto connectorDir = tubeConnectorDir(tileMap, xyz());
	mSprite.play(getAnimationName(connectorDir));
}
