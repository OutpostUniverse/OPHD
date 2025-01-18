#pragma once

#include "../Structure.h"

#include "../../Constants/Strings.h"


class CHAP : public Structure
{
public:
	CHAP() : Structure(StructureClass::LifeSupport, StructureID::SID_CHAP)
	{
	}
};
