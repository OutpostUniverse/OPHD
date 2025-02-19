#pragma once

#include "../Structure.h"


class Commercial : public Structure
{
public:
	Commercial() : Structure(
		StructureClass::Commercial,
		StructureID::SID_COMMERCIAL)
	{
	}
};
