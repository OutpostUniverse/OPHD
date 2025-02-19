#pragma once

#include "../Structure.h"


class University : public Structure
{
public:
	University() : Structure(
		StructureClass::University,
		StructureID::SID_UNIVERSITY)
	{
	}
};
