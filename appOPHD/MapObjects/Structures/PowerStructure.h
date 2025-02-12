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

	StringTable createInspectorViewTable() override;

	int energyProduced();

protected:
	virtual int calculateMaxEnergyProduction() = 0;
};
