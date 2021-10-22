#pragma once

#include "Structure.h"

#include "../../Constants/Strings.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/StringUtils.h>


class Recycling : public Structure
{
private:
	const int WasteProcessingCapacity = 30;
	const int ResidentialSupportCount = 10;

public:
	Recycling() : Structure(constants::Recycling,
		"structures/recycling.sprite",
		StructureClass::Recycling,
		StructureID::SID_RECYCLING)
	{
		maxAge(500);
		turnsToBuild(4);

		requiresCHAP(true);
		hasCrime(true);
	}


	/**
	 * Amount of waste the facility can process per turn.
	 */
	virtual int wasteProcessingCapacity() const
	{
		if (!operational()) {
			return 0;
		}

		return WasteProcessingCapacity;
	}


	/**
	 * Number of residential units the facility can support
	 * each turn.
	 */
	virtual int residentialSupportCount() const
	{
		if (!operational()) {
			return 0;
		}

		return ResidentialSupportCount;
	}


	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 2);

		stringTable[{0, 0}].text = "Max Residents Supported:";
		stringTable[{1, 0}].text = NAS2D::stringFrom(residentialSupportCount());

		stringTable[{0, 1}].text = "Max Waste Processing Capacity:";
		stringTable[{1, 1}].text = NAS2D::stringFrom(wasteProcessingCapacity());

		if (!operational()) {
			stringTable[{1, 0}].textColor = constants::WarningTextColor;
			stringTable[{1, 1}].textColor = constants::WarningTextColor;
		}

		return stringTable;
	}


protected:
	void defineResourceInput() override
	{
		energyRequired(4);
	}
};
