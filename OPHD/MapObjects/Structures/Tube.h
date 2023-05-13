#pragma once

#include "../Structure.h"

#include "../../Constants/Numbers.h"
#include "../../Constants/Strings.h"
#include "../../Common.h"


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
		requiresCHAP(false);
		state(StructureState::Operational);
		integrityDecayRate(0);
	}

private:

	static const std::string& getAnimationName(ConnectorDir dir, bool underground)
	{
		return *(
			(dir == ConnectorDir::CONNECTOR_INTERSECTION) ?
				(underground ? &constants::UgTubeIntersection : &constants::AgTubeIntersection) :
			(dir == ConnectorDir::CONNECTOR_RIGHT) ?
				(underground ? &constants::UgTubeRight : &constants::AgTubeRight) :
			(dir == ConnectorDir::CONNECTOR_LEFT) ?
				(underground ? &constants::UgTubelLeft : &constants::AgTubeLeft) :
			throw std::runtime_error("Tried to create a Tube structure with invalid connector direction paramter."));
	}
};
