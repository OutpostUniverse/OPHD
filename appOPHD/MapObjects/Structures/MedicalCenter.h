#pragma once

#include "../Structure.h"


class MedicalCenter : public Structure
{
public:
	MedicalCenter() : Structure(
		StructureClass::MedicalCenter,
		StructureID::SID_MEDICAL_CENTER)
	{
	}
};
