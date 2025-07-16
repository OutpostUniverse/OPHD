#include "Tube.h"

#include "../../Map/Tile.h"
#include "../../Constants/Strings.h"

#include <stdexcept>


Tube::Tube(Tile& tile, ConnectorDir dir) :
	Structure{
		StructureClass::Tube,
		StructureID::SID_TUBE,
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
		(dir == ConnectorDir::CONNECTOR_INTERSECTION) ?
			(underground ? &constants::UgTubeIntersection : &constants::AgTubeIntersection) :
		(dir == ConnectorDir::CONNECTOR_EAST_WEST) ?
			(underground ? &constants::UgTubeRight : &constants::AgTubeRight) :
		(dir == ConnectorDir::CONNECTOR_NORTH_SOUTH) ?
			(underground ? &constants::UgTubelLeft : &constants::AgTubeLeft) :
		throw std::runtime_error("Tried to create a Tube structure with invalid connector direction parameter."));
}
