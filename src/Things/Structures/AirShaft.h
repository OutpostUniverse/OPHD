#pragma once

#include "Structure.h"


class AirShaft: public Structure
{
public:
	AirShaft():	Structure(constants::AIR_SHAFT, "structures/air_shaft.sprite", CLASS_TUBE)
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL);
		maxAge(400);

		connectorDirection(CONNECTOR_VERTICAL);

		requiresCHAP(false);
	}


	virtual ~AirShaft()	{}

	void ug() { sprite().play(constants::STRUCTURE_STATE_OPERATIONAL_UG); }

	virtual void forced_state_change(StructureState _s) { if (_ug) { return; } }

protected:
private:
	bool _ug = false;
};
