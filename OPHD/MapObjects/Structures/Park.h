#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class Park : public Structure
{
public:
	Park() : Structure(
		StructureClass::Park,
		StructureID::SID_PARK)
	{
	}
};
