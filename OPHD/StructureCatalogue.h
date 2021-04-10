#pragma once

#include "Common.h"
#include "Constants.h"
#include "Things/Structures/Structures.h"
#include "StorableResources.h"

#include <array>

/** 
 * \class	StructureCatalogue
 * \brief	Provides a means of instantiating new structures and getting build
 *			cost / recycle value / population requirements.
 * 
 * StructureCatalogue is implemented as a static class and should never be
 * instantiated.
 * 
 * \note	StructureCatalogue::init() must be called prior to use.
 * 
 */
class StructureCatalogue
{
public:
	static void init();

	static Structure* get(StructureTypeID type);

	static const PopulationRequirements& populationRequirements(StructureTypeID type);
	static const StorableResources& costToBuild(StructureTypeID type);
	static const StorableResources recyclingValue(StructureTypeID type);

	static bool canBuild(const StorableResources& source, StructureTypeID type);

private:
	StructureCatalogue() {} // Explicitly declared private to prevent instantiation.
	~StructureCatalogue() {} // Explicitly declared private to prevent instantiation.

	static void buildCostTable();
	static void buildPopulationRequirementsTable();
	static void buildRecycleValueTable();

	static StorableResources recycleValue(StructureTypeID type, float percent);

private:
	static std::array<StorableResources, StructureTypeID::SID_COUNT> mStructureCostTable;
	static std::array<StorableResources, StructureTypeID::SID_COUNT> mStructureRecycleValueTable;
	static std::array<PopulationRequirements, StructureTypeID::SID_COUNT> mPopulationRequirementsTable;
};
