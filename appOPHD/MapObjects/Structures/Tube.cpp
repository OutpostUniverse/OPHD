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
		getAnimationName(dir, tile.isSurface()),
	}
{
	connectorDirection(dir);
	mStructureState = StructureState::Operational;
}


const std::string& Tube::getAnimationName(ConnectorDir dir, bool isSurface)
{
	return *(
		(dir == ConnectorDir::Intersection) ?
			(isSurface ? &constants::AgTubeIntersection : &constants::UgTubeIntersection) :
		(dir == ConnectorDir::EastWest) ?
			(isSurface ? &constants::AgTubeRight : &constants::UgTubeRight) :
		(dir == ConnectorDir::NorthSouth) ?
			(isSurface ? &constants::AgTubeLeft : &constants::UgTubeLeft) :
		throw std::runtime_error("Tried to create a Tube structure with invalid connector direction parameter."));
}
