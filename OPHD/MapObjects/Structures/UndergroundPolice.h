#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


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
