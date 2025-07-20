#pragma once

#include "../Structure.h"


class AirShaft : public Structure
{
public:
	AirShaft(Tile& tile);

	void underground();

	void forced_state_change(StructureState, DisabledReason, IdleReason) override;
};
