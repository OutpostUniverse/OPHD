#pragma once

#include "../../StructureComponent.h"
#include "../../Constants.h"
#include <string>

/**
 * \class	PowerStructure
 * \brief	Component class for structures whose purpose is power production
 */
class PowerStructure : public StructureComponent
{
public:
	static constexpr ComponentTypeID componentTypeID = 200;

	PowerStructure(Structure* structure) : StructureComponent(structure) {}

	// TODO (#843) Replace by deserialization
	void initialize(int baseProductionRate, float solarProductionRate)
	{
		mBaseProductionRate = baseProductionRate;
		mSolarProductionRate = solarProductionRate;
	}

	StringTable createInspectorViewTable() override
	{
		StringTable stringTable(2, 1);

		stringTable[{0, 0}].text = "Power Produced:";

		// TODO: How do we access planet attributes from here?
		//       Seems like that should be accessible through a global variable, global function,
		//       or through a static class. createInspectorViewTable shouldn't take it as argument
		//       since it's use is too specific to power structures.
		//       The GUI will show inaccurate production information until this is fixed.
		float solarFactor = 1.0f;
		auto produced = energyProduced(solarFactor);
		auto maxRate = productionRate(solarFactor);

		stringTable[{1, 0}].text = std::to_string(produced) + " / " + std::to_string(maxRate);

		if (produced == 0)
		{
			stringTable[{1, 0}].textColor = constants::WARNING_TEXT_COLOR;
		}

		return stringTable;
	}

	int energyProduced(float solarFactor)
	{
		if (structure().operational())
		{
			return productionRate(solarFactor);
		}
		return 0;
	}

private:
	int productionRate(float solarFactor)
	{
		return mBaseProductionRate + int(mSolarProductionRate * solarFactor + 0.5f);
	}

	int mBaseProductionRate = 0;
	float mSolarProductionRate = 0;
};
