#include "Tube.h"

#include "../StructureState.h"
#include "../../Constants/Strings.h"

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
	Tube{tile, ConnectorDir::Intersection}
{
}


Tube::Tube(Tile& tile, ConnectorDir dir) :
	Structure{
		StructureID::Tube,
		tile,
		getAnimationName(dir),
	}
{
	Structure::connectorDirection(dir);
	mStructureState = StructureState::Operational;
}


void Tube::connectorDirection(ConnectorDir dir)
{
	Structure::connectorDirection(dir);
	mSprite.play(getAnimationName(dir));
}
