#pragma once

#include "../Structure.h"


class AirShaft : public Structure
{
public:
	AirShaft(Tile& tile);

	void forcedStateChange(StructureState, DisabledReason, IdleReason) override;
};
