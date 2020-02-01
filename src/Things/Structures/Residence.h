#pragma once

#include "Structure.h"

#include "../../Constants.h"


/**
 * \class	Residence
 * \brief	Base Residential structure.
 * 
 * Implements the base Residence structures. Upgraded residences should derive
 * from this class and override the definition of mCapacity.
 */
class Residence : public Structure
{
public:
	Residence() : Structure(constants::RESIDENCE, "structures/residential_1.sprite", CLASS_RESIDENCE)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(500);
		turnsToBuild(2);

		requiresCHAP(true);
	}

	virtual ~Residence()
	{}


	int capacity() const { return mCapacity; }

protected:
	virtual void think()
	{}

	virtual void defineResourceInput()
	{
		resourcesIn().energy(2);
	}

	virtual void defineResourceOutput()
	{}

	int calculateIntegrity() override {
		return integrity() - calculateIntegrityDecayRate();
	}


	int calculateIntegrityDecayRate() override {
		return 0;
	}

protected:
	int		mCapacity = 25;		/**< Override this value in derived residences.*/

};
