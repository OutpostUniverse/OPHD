#pragma once

#include "../Structure.h"

#include <array>


/**
 * Virtual class for structures whose primary purpose is ore processing
 *
 * \note	OreRefining is an abstract class
 */
class OreRefining : public Structure
{
public:
	OreRefining(StructureClass structureClass, StructureID id);

	StringTable createInspectorViewTable() const override;

protected:
	std::array<int, 4> OreConversionDivisor{2, 2, 3, 3};

	StorableResources storageCapacities() const;

	/**
	 * Capacity of an individual type of refined resource
	 */
	int individualMaterialCapacity() const;

	void think() override;

	virtual void updateProduction();

private:
	std::string writeStorageAmount(int storageAmount) const;
};
