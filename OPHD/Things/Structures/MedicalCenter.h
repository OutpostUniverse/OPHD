#pragma once

#include "Structure.h"

#include "../../Constants/Strings.h"


class MedicalCenter : public Structure
{
public:
	MedicalCenter() : Structure(constants::MedicalCenter,
		"structures/medical.sprite",
		StructureClass::MedicalCenter,
		StructureID::SID_MEDICAL_CENTER)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
		hasCrime(true);
	}

protected:
	void defineResourceInput() override
	{
		energyRequired(5);
	}
};
