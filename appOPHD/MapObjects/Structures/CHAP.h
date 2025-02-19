#pragma once

#include "../Structure.h"


class CHAP : public Structure
{
public:
	CHAP() : Structure(StructureClass::LifeSupport, StructureID::SID_CHAP)
	{
	}
};
