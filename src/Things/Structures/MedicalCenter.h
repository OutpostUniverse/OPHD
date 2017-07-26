#pragma once

#include "Structure.h"

#include "../../Constants.h"

class MedicalCenter : public Structure
{
public:
	MedicalCenter() : Structure(constants::MEDICAL_CENTER, "structures/medical.sprite", CLASS_MEDICAL_CENTER)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}


	virtual ~MedicalCenter()
	{}

protected:

	virtual void think()
	{
	}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(1);
	}

	virtual void defineResourceOutput()
	{}

private:

};
