#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class University : public Structure
{
public:
	University() : Structure(
		StructureClass::University,
		StructureID::SID_UNIVERSITY)
	{
	}
};
