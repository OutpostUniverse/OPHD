#pragma once

#include "../Structure.h"


class Road : public Structure
{
public:
	Road() : Structure(
		StructureClass::Road,
		StructureID::SID_ROAD)
	{
	}
};
