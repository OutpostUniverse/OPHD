#pragma once

#include "Structure.h"

#include "../../Constants.h"


class AirShaft: public Structure
{
public:
	AirShaft() : Structure(constants::AIR_SHAFT, "structures/air_shaft.sprite",
		constants::STRUCTURE_STATE_OPERATIONAL,
		StructureClass::Tube,
		StructureID::SID_AIR_SHAFT)
	{
		connectorDirection(ConnectorDir::CONNECTOR_VERTICAL);

		requiresCHAP(false);
		state(StructureState::Operational);
	}

	void ug()
	{
		sprite().play(constants::STRUCTURE_STATE_OPERATIONAL_UG);
		mIsUnderground = true;
	}

	void forced_state_change(StructureState, DisabledReason, IdleReason) override
	{
		if (mIsUnderground)
		{
			return;
		}
	}

private:
	bool mIsUnderground = false;
};
