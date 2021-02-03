#pragma once

#include "Structure.h"

#include "../../Constants.h"

class Recycling : public Structure
{
private:
	const int WasteProcessingCapacity = 30;
	const int ResidentialSupportCount = 10;

public:
	Recycling() : Structure(constants::RECYCLING,
		"structures/recycling.sprite",
		StructureClass::Recycling,
		StructureID::SID_RECYCLING)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
	}


	/**
	 * Amount of waste the facility can process per turn.
	 */
	virtual int wasteProcessingCapacity() const
	{
		return WasteProcessingCapacity;
	}
	
	
	/**
	 * Number of residential units the facility can support
	 * each turn.
	 */
	virtual int residentialSupportCount() const
	{
	return ResidentialSupportCount;
	}


protected:
	void defineResourceInput() override
	{
		energyRequired(4);
	}
};
