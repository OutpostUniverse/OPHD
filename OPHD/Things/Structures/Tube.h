#pragma once

#include "Structure.h"

#include "../../Common.h"

/**
 * Implements a Tube Structure.
 */
class Tube : public Structure
{
public:
	Tube(ConnectorDir dir, bool underground) :
		Structure(constants::TUBE, "structures/tubes.sprite",
			getAnimationName(dir, underground),
			StructureClass::Tube,
			StructureID::SID_TUBE)
	{
		connectorDirection(dir);
		requiresCHAP(false);
		state(StructureState::Operational);
	}

private:

	static const std::string& getAnimationName(ConnectorDir dir, bool underground)
	{
		return *(
			(dir == ConnectorDir::CONNECTOR_INTERSECTION) ?
				(underground ? &constants::UG_TUBE_INTERSECTION : &constants::AG_TUBE_INTERSECTION) :
			(dir == ConnectorDir::CONNECTOR_RIGHT) ?
				(underground ? &constants::UG_TUBE_RIGHT : &constants::AG_TUBE_RIGHT) :
			(dir == ConnectorDir::CONNECTOR_LEFT) ?
				(underground ? &constants::UG_TUBE_LEFT : &constants::AG_TUBE_LEFT) :
			throw std::runtime_error("Tried to create a Tube structure with invalid connector direction paramter."));
	}
};
