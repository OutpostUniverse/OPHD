#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


/**
 * Implements a Tube Structure.
 */
class Tube : public Structure
{
public:
	Tube(ConnectorDir dir, bool underground) :
		Structure(
			getAnimationName(dir, underground),
			StructureClass::Tube,
			StructureID::SID_TUBE)
	{
		connectorDirection(dir);
		state(StructureState::Operational);
	}

private:

	static const std::string& getAnimationName(ConnectorDir dir, bool underground)
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
};
