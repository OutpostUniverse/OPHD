#pragma once

#include "../Structure.h"


class RecreationCenter : public Structure
{
public:
	RecreationCenter() : Structure(
		StructureClass::RecreationCenter,
		StructureID::SID_RECREATION_CENTER)
	{
	}
};
