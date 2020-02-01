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
	{}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(5);
	}

	virtual void defineResourceOutput()
	{}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

private:

};
