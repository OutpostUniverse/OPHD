#pragma once

#include "Structure.h"


class AirShaft: public Structure
{
public:
	AirShaft():	Structure(constants::AIR_SHAFT, "structures/air_shaft.sprite", StructureClass::CLASS_TUBE)
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		maxAge(400);

		connectorDirection(ConnectorDir::CONNECTOR_VERTICAL);

		requiresCHAP(false);
	}

	void ug()
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL_UG);
		_ug = true;
	}

	void forced_state_change(StructureState, DisabledReason, IdleReason) override
	{
		if (_ug)
		{
			return;
		}
	}

private:
	bool _ug = false;
};
