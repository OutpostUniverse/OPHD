#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class Commercial : public Structure
{
public:
	Commercial() : Structure(
		StructureClass::Commercial,
		StructureID::SID_COMMERCIAL)
	{
	}
};
