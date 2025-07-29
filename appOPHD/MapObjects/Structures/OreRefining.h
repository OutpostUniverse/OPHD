#pragma once

#include "../Structure.h"


/**
 * Virtual class for structures whose primary purpose is ore processing
 *
 * \note	OreRefining is an abstract class
 */
class OreRefining : public Structure
{
public:
	OreRefining(StructureID id, Tile& tile);

	StringTable createInspectorViewTable() const override;

protected:
	StorableResources storageCapacities() const;

	void think() override;

	virtual void updateProduction();

private:
	std::string writeStorageAmount(int storageAmount) const;
};
