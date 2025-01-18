#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class RedLightDistrict : public Structure
{
public:
	RedLightDistrict() : Structure(
		StructureClass::Residence,
		StructureID::SID_RED_LIGHT_DISTRICT)
	{
	}
};
