#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class RecreationCenter : public Structure
{
public:
	RecreationCenter() : Structure(
		StructureClass::RecreationCenter,
		StructureID::SID_RECREATION_CENTER)
	{
	}
};
