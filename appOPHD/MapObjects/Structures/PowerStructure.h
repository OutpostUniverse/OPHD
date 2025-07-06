#pragma once

#include "../Structure.h"


/**
 * Virtual class for structures whose primary purpose is power production
 *
 * \note	PowerStructure is an abstract class
 */
class PowerStructure : public Structure
{
public:
	PowerStructure(StructureClass structureClass, StructureID id);

	StringTable createInspectorViewTable() const override;

	int energyProduced() const;

protected:
	virtual int calculateMaxEnergyProduction() const = 0;
};
