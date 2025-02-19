#pragma once

#include "../Structure.h"


class Nursery : public Structure
{
public:
	Nursery() : Structure(
		StructureClass::Nursery,
		StructureID::SID_NURSERY)
	{
	}
};
