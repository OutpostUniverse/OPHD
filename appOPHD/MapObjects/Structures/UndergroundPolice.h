#pragma once

#include "../Structure.h"


class UndergroundPolice : public Structure
{
public:
	UndergroundPolice() : Structure(
		StructureClass::UndergroundPolice,
		StructureID::SID_UNDERGROUND_POLICE)
	{
	}

	int getRange() const
	{
		return 5;
	}
};
