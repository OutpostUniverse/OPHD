#include "Tube.h"

#include "../StructureState.h"
#include "../../Map/Tile.h"
#include "../../Constants/Strings.h"

#include <libOPHD/EnumStructureID.h>
#include <libOPHD/EnumConnectorDir.h>

#include <stdexcept>


Tube::Tube(Tile& tile) :
	Tube{tile, ConnectorDir::Intersection}
{
}


Tube::Tube(Tile& tile, ConnectorDir dir) :
	Structure{
		StructureID::Tube,
		tile,
		getAnimationName(dir, tile.depth() != 0),
	}
{
	connectorDirection(dir);
	state(StructureState::Operational);
}


const std::string& Tube::getAnimationName(ConnectorDir dir, bool underground)
{
	return *(
		(dir == ConnectorDir::Intersection) ?
			(underground ? &constants::UgTubeIntersection : &constants::AgTubeIntersection) :
		(dir == ConnectorDir::EastWest) ?
			(underground ? &constants::UgTubeRight : &constants::AgTubeRight) :
		(dir == ConnectorDir::NorthSouth) ?
			(underground ? &constants::UgTubelLeft : &constants::AgTubeLeft) :
		throw std::runtime_error("Tried to create a Tube structure with invalid connector direction parameter."));
}
