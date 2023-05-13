#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class Nursery : public Structure
{
public:
	Nursery() : Structure(
		StructureClass::Nursery,
		StructureID::SID_NURSERY)
	{
	}
};
