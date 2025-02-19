#pragma once

#include "../Structure.h"


class AirShaft : public Structure
{
public:
	AirShaft();

	void ug();

	void forced_state_change(StructureState, DisabledReason, IdleReason) override;

private:
	bool mIsUnderground = false;
};
