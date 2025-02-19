#pragma once

#include "../Structure.h"


class Park : public Structure
{
public:
	Park() : Structure(
		StructureClass::Park,
		StructureID::SID_PARK)
	{
	}
};
