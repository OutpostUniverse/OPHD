#pragma once

#include "Structure.h"

class CHAP : public Structure
{
public:
	CHAP() : Structure(constants::CHAP, "structures/chap.sprite", StructureClass::LifeSupport, StructureTypeID::SID_CHAP)
	{
		maxAge(600);
		turnsToBuild(5);

		requiresCHAP(false);
	}

protected:
	void defineResourceInput() override
	{
		resourcesIn({ 2, 0, 1, 1 });
		energyRequired(10);
	}
};
